/*
 * Copyright (c) 1997 - 2005 Kungliga Tekniska Högskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "kadmin_locl.h"
#include <krb5-private.h>

static kadm5_ret_t check_aliases(kadm5_server_context *,
                                 kadm5_principal_ent_rec *,
                                 kadm5_principal_ent_rec *);

/*
 * All the iter_cb stuff is about online listing of principals via
 * kadm5_iter_principals().  Search for "LIST" to see more commentary.
 */
struct iter_cb_data {
    krb5_context context;
    krb5_auth_context ac;
    krb5_storage *rsp;
    kadm5_ret_t ret;
    size_t n;
    size_t i;
    int fd;
    unsigned int initial:1;
    unsigned int stop:1;
};

/*
 * This function sends the current chunk of principal listing and checks if the
 * client requested that the listing stop.
 */
static int
iter_cb_send_now(struct iter_cb_data *d)
{
    struct timeval tv;
    krb5_data out;

    krb5_data_zero(&out);

    if (!d->stop) {
        fd_set fds;
        int nfds;

        /*
         * The client can send us one message to interrupt the iteration.
         *
         * TODO: Maybe we should have the client send a message every N chunks
         *       so we can clock the listing and have a chance to receive any
         *       interrupt message from the client?
         */
        FD_ZERO(&fds);
        FD_SET(d->fd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        nfds = select(d->fd + 1, &fds, NULL, NULL, &tv);
        if (nfds == -1) {
            d->ret = errno;
        } else if (nfds > 0) {
            /*
             * And it did.  We'll throw this message away.  It should be a NOP
             * call, which we'd throw away anyways.  If the client's stop
             * message arrives after we're done anyways, well, it will be
             * processed as a NOP and thrown away.
             */
            d->stop = 1;
            d->ret = krb5_read_priv_message(d->context, d->ac, &d->fd, &out);
            krb5_data_free(&out);
            if (d->ret == HEIM_ERR_EOF)
                exit(0);
        }
    }
    d->i = 0;
    d->ret = krb5_storage_to_data(d->rsp, &out);
    if (d->ret == 0)
        d->ret = krb5_write_priv_message(d->context, d->ac, &d->fd, &out);
    krb5_data_free(&out);
    krb5_storage_free(d->rsp);
    if ((d->rsp = krb5_storage_emem()) == NULL)
        return krb5_enomem(d->context);
    return d->ret;
}

static int
iter_cb(void *cbdata, const char *p)
{
    struct iter_cb_data *d = cbdata;
    krb5_error_code ret = 0;
    size_t n = d->n;

    /* Convince the compiler that `-(int)d->n' is defined */
    if (n == 0 || n > INT_MAX)
        return ERANGE;
    if (d->rsp == NULL && (d->rsp = krb5_storage_emem()) == NULL)
        return krb5_enomem(d->context);
    if (d->i == 0) {
        /* Every chunk starts with a result code */
        ret = krb5_store_int32(d->rsp, d->ret);
        if (ret)
            return ret;
        if (d->ret)
            return ret;
    }
    if (d->initial) {
        /*
         * We'll send up to `d->n' entries per-write.  We send a negative
         * number to indicate we accepted the client's proposal that we speak
         * the online LIST protocol.
         *
         * Note that if we're here then we've already placed a result code in
         * this reply (see above).
         */
        d->initial = 0;
        ret = krb5_store_int32(d->rsp, -(int)n);    /* Princs per-chunk */
        if (ret == 0)
            ret = iter_cb_send_now(d);
        if (ret)
            return ret;
        /*
         * Now that we've sent the acceptance reply, put a result code as the
         * first thing in the next reply, which will have the first chunk of
         * the listing.
         */
        ret = krb5_store_int32(d->rsp, d->ret);
        if (ret)
            return ret;
        if (d->ret)
            return ret;
    }

    if (p) {
        ret = krb5_store_string(d->rsp, p);
        d->i++;
    } else {
        /*
         * We get called with `p == NULL' when the listing is done.  This
         * forces us to iter_cb_send_now(d) below, but also forces us to have a
         * properly formed reply (i.e., that we have a result code as the first
         * item), even if the chunk is otherwise empty (`d->i == 0').
         */
        d->i = n;
    }

    if (ret == 0 && d->i == n)
        ret = iter_cb_send_now(d); /* Chunk finished; send it */
    if (d->stop)
        return EINTR;
    return ret;
}

static kadm5_ret_t
kadmind_dispatch(void *kadm_handlep, krb5_boolean initial,
		 krb5_data *in, krb5_auth_context ac, int fd,
                 krb5_data *out, int readonly)
{
    kadm5_ret_t ret = 0;
    kadm5_ret_t ret_sp = 0;
    int32_t cmd, mask, kvno, tmp;
    kadm5_server_context *contextp = kadm_handlep;
    char client[128], name[128], name2[128];
    const char *op = "";
    krb5_principal princ = NULL, princ2 = NULL;
    kadm5_principal_ent_rec ent, ent_prev;
    char *password = NULL, *expression;
    krb5_keyblock *new_keys;
    krb5_key_salt_tuple *ks_tuple = NULL;
    int keepold = FALSE;
    int n_ks_tuple = 0;
    int n_keys;
    char **princs;
    int n_princs;
    int keys_ok = 0;
    krb5_storage *rsp; /* response goes here */
    krb5_storage *sp;
    int len;

    memset(&ent, 0, sizeof(ent));
    memset(&ent_prev, 0, sizeof(ent_prev));
    krb5_data_zero(out);

    rsp = krb5_storage_emem();
    if (rsp == NULL)
	return krb5_enomem(contextp->context);

    sp = krb5_storage_from_data(in);
    if (sp == NULL) {
	krb5_storage_free(rsp);
	return krb5_enomem(contextp->context);
    }

    ret = krb5_unparse_name_fixed(contextp->context, contextp->caller,
				  client, sizeof(client));
    if (ret == 0)
        ret = krb5_ret_int32(sp, &cmd);
    if (ret) {
        ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
        goto fail;
    }

    switch(cmd){
    case kadm_nop:{
        /*
         * In the future we could use this for versioning.
         *
         * We used to respond to NOPs with KADM5_FAILURE.  Now we respond with
         * zero.  In the future we could send back a protocol version number
         * and use NOPs for protocol version negotiation.
         *
         * In the meantime, this gets called only if a client wants to
         * interrupt a long-running LIST operation.
         */
	op = "NOP";
	ret = krb5_ret_int32(sp, &tmp);
        if (ret == 0 && tmp == 0) {
            /*
             * Reply not wanted.  This would be a LIST interrupt request.
             */
            krb5_storage_free(rsp);
            krb5_storage_free(sp);
            return 0;
        }
	ret_sp = krb5_store_int32(rsp, ret = 0);
        break;
    }
    case kadm_get:{
	op = "GET";
	ret = krb5_ret_principal(sp, &princ);
	if (ret) {
            ret_sp = krb5_store_int32(rsp, KADM5_UNK_PRINC);
	    goto fail;
        }
	ret = krb5_ret_int32(sp, &mask);
	if (ret) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    goto fail;
        }

	mask |= KADM5_PRINCIPAL;
	krb5_unparse_name_fixed(contextp->context, princ, name, sizeof(name));
	krb5_warnx(contextp->context, "%s: %s %s", client, op, name);

        /* If the caller doesn't have KADM5_PRIV_GET, we're done. */
	ret = _kadm5_acl_check_permission(contextp, KADM5_PRIV_GET, princ);
        if (ret) {
            ret_sp = krb5_store_int32(rsp, ret);
	    goto fail;
        }

        /* Then check to see if it is ok to return keys */
        if ((mask & KADM5_KEY_DATA) != 0) {
            ret = _kadm5_acl_check_permission(contextp, KADM5_PRIV_GET_KEYS,
                                              princ);
            if (ret == 0) {
                keys_ok = 1;
            } else if ((mask == (KADM5_PRINCIPAL|KADM5_KEY_DATA)) ||
                       (mask == (KADM5_PRINCIPAL|KADM5_KVNO|KADM5_KEY_DATA))) {
                /*
                 * Requests for keys will get bogus keys, which is useful if
                 * the client just wants to see what (kvno, enctype)s the
                 * principal has keys for, but terrible if the client wants to
                 * write the keys into a keytab or modify the principal and
                 * write the bogus keys back to the server.
                 *
                 * We use a heuristic to detect which case we're handling here.
                 * If the client only asks for the flags in the above
                 * condition, then it's very likely a kadmin ext_keytab,
                 * add_enctype, or other request that should not see bogus
                 * keys.  We deny them.
                 *
                 * The kadmin get command can be coaxed into making a request
                 * with the same mask.  But the default long and terse output
                 * modes request other things too, so in all likelihood this
                 * heuristic will not hurt any kadmin get uses.
                 */
                ret_sp = krb5_store_int32(rsp, ret);
                goto fail;
            }
        }

	ret = kadm5_get_principal(kadm_handlep, princ, &ent, mask);
	ret_sp = krb5_store_int32(rsp, ret);
	if (ret == 0) {
	    if (ret_sp == 0 && keys_ok)
		ret_sp = kadm5_store_principal_ent(rsp, &ent);
	    else if (ret_sp == 0)
		ret_sp = kadm5_store_principal_ent_nokeys(rsp, &ent);
	}
        kadm5_free_principal_ent(kadm_handlep, &ent);
	break;
    }
    case kadm_delete:{
	op = "DELETE";
	if (readonly) {
            ret_sp = krb5_store_int32(rsp, ret = KADM5_READ_ONLY);
            goto fail;
        }
	ret = krb5_ret_principal(sp, &princ);
        if (ret == 0)
            ret = krb5_unparse_name_fixed(contextp->context, princ, name, sizeof(name));
        if (ret == 0) {
            ret = _kadm5_acl_check_permission(contextp, KADM5_PRIV_DELETE, princ);
            krb5_warnx(contextp->context, "%s: %s %s (%s)", client, op, name,
                       ret == 0 ? "granted" : "denied");
        }

        /*
         * There's no need to check that the caller has permission to
         * delete the victim principal's aliases.
         */
        if (ret == 0)
            ret = kadm5_delete_principal(kadm_handlep, princ);
	ret_sp = krb5_store_int32(rsp, ret);
	break;
    }
    case kadm_create:{
	op = "CREATE";
	if (readonly) {
            ret_sp = krb5_store_int32(rsp, ret = KADM5_READ_ONLY);
            goto fail;
        }
	ret = kadm5_ret_principal_ent(sp, &ent);
	if(ret) {
            ret_sp = krb5_store_int32(rsp, ret);
	    goto fail;
        }
	ret = krb5_ret_int32(sp, &mask);
	if(ret){
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    kadm5_free_principal_ent(kadm_handlep, &ent);
	    goto fail;
	}
	ret = krb5_ret_string(sp, &password);
	if(ret){
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    kadm5_free_principal_ent(kadm_handlep, &ent);
	    goto fail;
	}
	krb5_unparse_name_fixed(contextp->context, ent.principal,
				name, sizeof(name));
	krb5_warnx(contextp->context, "%s: %s %s", client, op, name);
	ret = _kadm5_acl_check_permission(contextp, KADM5_PRIV_ADD,
					  ent.principal);
	if(ret){
            ret_sp = krb5_store_int32(rsp, ret);
	    kadm5_free_principal_ent(kadm_handlep, &ent);
	    goto fail;
	}
        if ((mask & KADM5_TL_DATA)) {
            /*
             * Also check that the caller can create the aliases, if the
             * new principal has any.
             */
            ret = check_aliases(contextp, &ent, NULL);
            if (ret) {
                ret_sp = krb5_store_int32(rsp, KADM5_BAD_PRINCIPAL);
                kadm5_free_principal_ent(kadm_handlep, &ent);
                goto fail;
            }
        }
	ret = kadm5_create_principal(kadm_handlep, &ent,
				     mask, password);
	kadm5_free_principal_ent(kadm_handlep, &ent);
	ret_sp = krb5_store_int32(rsp, ret);
	break;
    }
    case kadm_modify:{
	op = "MODIFY";
	if (readonly) {
            ret_sp = krb5_store_int32(rsp, ret = KADM5_READ_ONLY);
            goto fail;
        }
	ret = kadm5_ret_principal_ent(sp, &ent);
	if(ret) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    goto fail;
        }
	ret = krb5_ret_int32(sp, &mask);
	if(ret){
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    kadm5_free_principal_ent(contextp, &ent);
	    goto fail;
	}
	krb5_unparse_name_fixed(contextp->context, ent.principal,
				name, sizeof(name));
	krb5_warnx(contextp->context, "%s: %s %s", client, op, name);
	ret = _kadm5_acl_check_permission(contextp, KADM5_PRIV_MODIFY,
					  ent.principal);
	if(ret){
            ret_sp = krb5_store_int32(rsp, ret);
	    kadm5_free_principal_ent(contextp, &ent);
	    goto fail;
	}
        if ((mask & KADM5_TL_DATA)) {
            /*
             * Also check that the caller can create aliases that are in
             * the new entry but not the old one.  There's no need to
             * check that the caller can delete aliases it wants to
             * drop.  See also handling of rename.
             */
            ret = kadm5_get_principal(kadm_handlep, ent.principal, &ent_prev, mask);
            if (ret) {
                ret_sp = krb5_store_int32(rsp, ret);
                kadm5_free_principal_ent(contextp, &ent);
                goto fail;
            }
            ret = check_aliases(contextp, &ent, &ent_prev);
            kadm5_free_principal_ent(contextp, &ent_prev);
            if (ret) {
                ret_sp = krb5_store_int32(rsp, KADM5_BAD_PRINCIPAL);
                kadm5_free_principal_ent(contextp, &ent);
                goto fail;
            }
        }
	ret = kadm5_modify_principal(kadm_handlep, &ent, mask);
	kadm5_free_principal_ent(kadm_handlep, &ent);
	ret_sp = krb5_store_int32(rsp, ret);
	break;
    }
    case kadm_prune:{
        op = "PRUNE";
	if (readonly) {
            ret_sp = krb5_store_int32(rsp, ret = KADM5_READ_ONLY);
            goto fail;
        }
        ret = krb5_ret_principal(sp, &princ);
        if (ret == 0)
            ret = krb5_ret_int32(sp, &kvno);
        if (ret == HEIM_ERR_EOF) {
            kvno = 0;
        } else if (ret) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
            goto fail;
        }
        krb5_unparse_name_fixed(contextp->context, princ, name, sizeof(name));
        krb5_warnx(contextp->context, "%s: %s %s", client, op, name);
        ret = _kadm5_acl_check_permission(contextp, KADM5_PRIV_CPW, princ);
        if (ret) {
            ret_sp = krb5_store_int32(rsp, ret);
            goto fail;
        }

        ret = kadm5_prune_principal(kadm_handlep, princ, kvno);
        ret_sp = krb5_store_int32(rsp, ret);
        break;
    }
    case kadm_rename:{
	op = "RENAME";
	if (readonly) {
            ret_sp = krb5_store_int32(rsp, ret = KADM5_READ_ONLY);
            goto fail;
        }
	ret = krb5_ret_principal(sp, &princ);
        if (ret == 0)
            ret = krb5_ret_principal(sp, &princ2);
	if (ret) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    goto fail;
        }

	krb5_unparse_name_fixed(contextp->context, princ, name, sizeof(name));
	krb5_unparse_name_fixed(contextp->context, princ2,
                                name2, sizeof(name2));
	krb5_warnx(contextp->context, "%s: %s %s -> %s",
		   client, op, name, name2);
	ret = _kadm5_acl_check_permission(contextp,
					  KADM5_PRIV_ADD,
					  princ2);
        if (ret == 0) {
            /*
             * Also require modify for the principal.  For backwards
             * compatibility, allow delete permission on the old name to
             * cure lack of modify permission on the old name.
             */
            ret = _kadm5_acl_check_permission(contextp,
                                              KADM5_PRIV_MODIFY,
                                              princ);
            if (ret) {
                ret = _kadm5_acl_check_permission(contextp,
                                                  KADM5_PRIV_DELETE,
                                                  princ);
            }
        }
	if (ret) {
            ret_sp = krb5_store_int32(rsp, ret);
	    goto fail;
        }

	ret = kadm5_rename_principal(kadm_handlep, princ, princ2);
	ret_sp = krb5_store_int32(rsp, ret);
	break;
    }
    case kadm_chpass:{
	krb5_boolean is_self_cpw, allow_self_cpw;

	op = "CHPASS";
	if (readonly) {
            ret_sp = krb5_store_int32(rsp, ret = KADM5_READ_ONLY);
            goto fail;
        }
	ret = krb5_ret_principal(sp, &princ);
        if (ret == 0)
            ret = krb5_ret_string(sp, &password);
        if (ret == 0)
            ret = krb5_ret_int32(sp, &keepold);
        if (ret == HEIM_ERR_EOF)
            ret = 0;
        if (ret == 0) {
            ret = krb5_unparse_name_fixed(contextp->context, princ, name, sizeof(name));
            if (ret == 0)
                krb5_warnx(contextp->context, "%s: %s %s", client, op, name);
        }
	if (ret) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    goto fail;
        }

	/*
	 * Change password requests are subject to ACLs unless the principal is
	 * changing their own password and the initial ticket flag is set, and
	 * the allow_self_change_password configuration option is TRUE.
	 */
	is_self_cpw =
	    krb5_principal_compare(contextp->context, contextp->caller, princ);
	allow_self_cpw =
	    krb5_config_get_bool_default(contextp->context, NULL, TRUE,
					 "kadmin", "allow_self_change_password", NULL);
	if (!(is_self_cpw && initial && allow_self_cpw)) {
	    ret = _kadm5_acl_check_permission(contextp, KADM5_PRIV_CPW, princ);
	    if (ret) {
                ret_sp = krb5_store_int32(rsp, ret);
		goto fail;
            }
	}

	ret = kadm5_chpass_principal_3(kadm_handlep, princ, keepold, 0, NULL,
				       password);
	ret_sp = krb5_store_int32(rsp, ret);
	break;
    }
    case kadm_chpass_with_key:{
	int i;
	krb5_key_data *key_data;
	int n_key_data;

	op = "CHPASS_WITH_KEY";
	if (readonly) {
            ret_sp = krb5_store_int32(rsp, ret = KADM5_READ_ONLY);
            goto fail;
        }
	ret = krb5_ret_principal(sp, &princ);
        if (ret == 0)
	    ret = krb5_ret_int32(sp, &n_key_data);
        if (ret == 0) {
	    ret = krb5_ret_int32(sp, &keepold);
	    if (ret == HEIM_ERR_EOF)
		ret = 0;
	}
	if (ret) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    goto fail;
        }

	/* n_key_data will be squeezed into an int16_t below. */
	if (n_key_data < 0 || n_key_data >= 1 << 16 ||
	    (size_t)n_key_data > UINT_MAX/sizeof(*key_data)) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    ret = ERANGE;
	    goto fail;
	}

	key_data = malloc (n_key_data * sizeof(*key_data));
	if (key_data == NULL && n_key_data != 0) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    ret = krb5_enomem(contextp->context);
	    goto fail;
	}

	for (i = 0; i < n_key_data; ++i) {
	    ret = kadm5_ret_key_data (sp, &key_data[i]);
	    if (ret) {
		int16_t dummy = i;

		kadm5_free_key_data (contextp, &dummy, key_data);
		free (key_data);
                ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
		goto fail;
	    }
	}

	/*
	 * The change is only allowed if the user is on the CPW ACL,
	 * this it to force password quality check on the user.
	 */

	ret = _kadm5_acl_check_permission(contextp, KADM5_PRIV_CPW, princ);
	ret_sp = krb5_unparse_name_fixed(contextp->context, princ, name, sizeof(name));
        if (ret_sp == 0)
            krb5_warnx(contextp->context, "%s: %s %s (%s)", client, op, name,
                       ret ? "denied" : "granted");
	if(ret) {
	    int16_t dummy = n_key_data;

	    kadm5_free_key_data (contextp, &dummy, key_data);
	    free (key_data);
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    goto fail;
	}
	ret = kadm5_chpass_principal_with_key_3(kadm_handlep, princ, keepold,
					        n_key_data, key_data);
	{
	    int16_t dummy = n_key_data;
	    kadm5_free_key_data (contextp, &dummy, key_data);
	}
	free (key_data);
	ret_sp = krb5_store_int32(rsp, ret);
	break;
    }
    case kadm_randkey:{
        size_t i;

	op = "RANDKEY";
	if (readonly) {
            ret_sp = krb5_store_int32(rsp, ret = KADM5_READ_ONLY);
            goto fail;
        }
	ret = krb5_ret_principal(sp, &princ);
	if (ret) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    goto fail;
        }
	krb5_unparse_name_fixed(contextp->context, princ, name, sizeof(name));
	krb5_warnx(contextp->context, "%s: %s %s", client, op, name);
	/*
	 * The change is allowed if at least one of:
	 * a) it's for the principal him/herself and this was an initial ticket
	 * b) the user is on the CPW ACL.
	 */

	if (initial
	    && krb5_principal_compare (contextp->context, contextp->caller,
				       princ))
	    ret = 0;
	else
	    ret = _kadm5_acl_check_permission(contextp, KADM5_PRIV_CPW, princ);

	if (ret) {
            ret_sp = krb5_store_int32(rsp, ret);
	    goto fail;
        }

	/*
	 * See comments in kadm5_c_randkey_principal() regarding the
	 * protocol.
	 */
	ret = krb5_ret_int32(sp, &keepold);
	if (ret != 0 && ret != HEIM_ERR_EOF) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    goto fail;
        }

	ret = krb5_ret_int32(sp, &n_ks_tuple);
	if (ret == HEIM_ERR_EOF) {
            const char *enctypes;
	    size_t n;

            enctypes = krb5_config_get_string(contextp->context, NULL,
                                              "realms",
                                              krb5_principal_get_realm(contextp->context,
                                                                       princ),
                                              "supported_enctypes", NULL);
            if (enctypes == NULL || enctypes[0] == '\0')
                enctypes = "aes128-cts-hmac-sha1-96";
            ret = krb5_string_to_keysalts2(contextp->context, enctypes,
                                           &n, &ks_tuple);
	    n_ks_tuple = n;
        }
        if (ret != 0) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE); /* XXX */
	    goto fail;
        }

        if (n_ks_tuple < 0) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE); /* XXX */
            ret = EOVERFLOW;
            goto fail;
        }
	free(ks_tuple);
        if ((ks_tuple = calloc(n_ks_tuple, sizeof (*ks_tuple))) == NULL) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
            ret = errno;
            goto fail;
        }

        for (i = 0; i < n_ks_tuple; i++) {
            ret = krb5_ret_int32(sp, &ks_tuple[i].ks_enctype);
            if (ret != 0) {
                ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
                free(ks_tuple);
                goto fail;
            }
            ret = krb5_ret_int32(sp, &ks_tuple[i].ks_salttype);
            if (ret != 0) {
                ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
                free(ks_tuple);
                goto fail;
            }
        }
	ret = kadm5_randkey_principal_3(kadm_handlep, princ, keepold,
					n_ks_tuple, ks_tuple, &new_keys,
					&n_keys);
        free(ks_tuple);

	ret_sp = krb5_store_int32(rsp, ret);
	if (ret == 0 && ret_sp == 0){
	    ret_sp = krb5_store_int32(rsp, n_keys);
	    for (i = 0; i < n_keys; i++){
                if (ret_sp == 0)
                    ret_sp = krb5_store_keyblock(rsp, new_keys[i]);
		krb5_free_keyblock_contents(contextp->context, &new_keys[i]);
	    }
	    free(new_keys);
	}
	break;
    }
    case kadm_get_privs:{
	uint32_t privs;
	ret = kadm5_get_privs(kadm_handlep, &privs);
	if (ret == 0)
	    ret_sp = krb5_store_uint32(rsp, privs);
	break;
    }
    case kadm_get_princs:{
	op = "LIST";
	ret = krb5_ret_int32(sp, &tmp);
	if (ret) {
            ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	    goto fail;
        }
        /* See kadm5_c_iter_principals() */
	if (tmp == 0x55555555) {
            /* Want online iteration */
	    ret = krb5_ret_string(sp, &expression);
	    if (ret) {
                ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
                goto fail;
            }
            if (expression[0] == '\0') {
                free(expression);
                expression = NULL;
            }
        } else if (tmp) {
	    ret = krb5_ret_string(sp, &expression);
	    if (ret) {
                ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
		goto fail;
            }
	}else
	    expression = NULL;
	krb5_warnx(contextp->context, "%s: %s %s", client, op,
		   expression ? expression : "*");
	ret = _kadm5_acl_check_permission(contextp, KADM5_PRIV_LIST, NULL);
	if(ret){
            ret_sp = krb5_store_int32(rsp, ret);
	    free(expression);
	    goto fail;
	}
        if (fd > -1 && tmp == 0x55555555) {
            struct iter_cb_data iter_cbdata;
            int n;

            /*
             * The client proposes that we speak the online variation of LIST
             * by sending a magic value in the int32 that is meant to be a
             * boolean for "an expression follows".  The client must send an
             * expression in this case because the server might be an old one,
             * so even if the caller to kadm5_get/iter_principals() passed NULL
             * for the expression, the client must send something ("*").
             *
             * The list of principals will be streamed in multiple replies.
             *
             * The first reply will have just a return code and a negative
             * count of maximum number of names per-subsequent reply.  See
             * `iter_cb()'.
             *
             * The second reply, third, .., nth replies will have a return code
             * followed by 50 names, except the last reply must have fewer than
             * 50 names -zero if need be- so the client can deterministically
             * notice the end of the stream.
             */

            n = list_chunk_size;
            if (n < 0)
                n = krb5_config_get_int_default(contextp->context, NULL, -1,
                                                "kadmin", "list_chunk_size", NULL);
            if (n < 0)
                n = 50;
            if (n > 500)
                n = 500;
            if ((iter_cbdata.rsp = krb5_storage_emem()) == NULL) {
                ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
                ret = krb5_enomem(contextp->context);
                goto fail;
            }
            iter_cbdata.context = contextp->context;
            iter_cbdata.initial = 1;
            iter_cbdata.stop = 0;
            iter_cbdata.ret = 0;
            iter_cbdata.ac = ac;
            iter_cbdata.fd = fd;
            iter_cbdata.n = n;
            iter_cbdata.i = 0;

            /*
             * All sending of replies will happen in iter_cb, except for the
             * final chunk with the final result code.
             */
            iter_cbdata.ret = kadm5_iter_principals(kadm_handlep, expression,
                                                     iter_cb, &iter_cbdata);
            /* Send terminating chunk */
            iter_cb(&iter_cbdata, NULL);
            /* Final result */
            ret = krb5_store_int32(rsp, iter_cbdata.ret);
            krb5_storage_free(iter_cbdata.rsp);
        } else {
            ret = kadm5_get_principals(kadm_handlep, expression, &princs, &n_princs);
            ret_sp = krb5_store_int32(rsp, ret);
            if (ret == 0 && ret_sp == 0) {
                int i;

                ret_sp = krb5_store_int32(rsp, n_princs);
                for (i = 0; ret_sp == 0 && i < n_princs; i++)
                    ret_sp = krb5_store_string(rsp, princs[i]);
                kadm5_free_name_list(kadm_handlep, princs, &n_princs);
            }
        }
        free(expression);
	break;
    }
    default:
	krb5_warnx(contextp->context, "%s: UNKNOWN OP %d", client, cmd);
	ret_sp = krb5_store_int32(rsp, KADM5_FAILURE);
	break;
    }

fail:
    if (password != NULL) {
	len = strlen(password);
	memset_s(password, len, 0, len);
	free(password);
    }
    krb5_storage_to_data(rsp, out);
    krb5_storage_free(rsp);
    krb5_storage_free(sp);
    krb5_free_principal(contextp->context, princ);
    krb5_free_principal(contextp->context, princ2);
    if (ret)
        krb5_warn(contextp->context, ret, "%s", op);
    if (out->length == 0)
        krb5_warn(contextp->context, ret, "%s: reply failed", op);
    else if (ret_sp)
        krb5_warn(contextp->context, ret, "%s: reply incomplete", op);
    if (ret_sp)
        return ret_sp;
    return 0;
}

struct iter_aliases_ctx {
    HDB_Ext_Aliases aliases;
    krb5_tl_data *tl;
    int alias_idx;
    int done;
};

static kadm5_ret_t
iter_aliases(kadm5_principal_ent_rec *from,
             struct iter_aliases_ctx *ctx,
             krb5_principal *out)
{
    HDB_extension ext;
    kadm5_ret_t ret;
    size_t size;

    *out = NULL;

    if (ctx->done > 0)
        return 0;
    if (from == NULL) {
        ctx->done = 1;
        return 0;
    }

    if (ctx->done == 0) {
        if (ctx->alias_idx < ctx->aliases.aliases.len) {
            *out = &ctx->aliases.aliases.val[ctx->alias_idx++];
            return 0;
        }
        /* Out of aliases in this TL, step to next TL */
        ctx->tl = ctx->tl->tl_data_next;
    } else if (ctx->done < 0) {
        /* Setup iteration context */
        memset(ctx, 0, sizeof(*ctx));
        ctx->done = 0;
        ctx->aliases.aliases.val = NULL;
        ctx->aliases.aliases.len = 0;
        ctx->tl = from->tl_data;
    }

    free_HDB_Ext_Aliases(&ctx->aliases);
    ctx->alias_idx = 0;

    /* Find TL with aliases */
    for (; ctx->tl != NULL; ctx->tl = ctx->tl->tl_data_next) {
        if (ctx->tl->tl_data_type != KRB5_TL_EXTENSION)
            continue;

        ret = decode_HDB_extension(ctx->tl->tl_data_contents,
                                   ctx->tl->tl_data_length,
                                   &ext, &size);
        if (ret)
            return ret;
        if (ext.data.element == choice_HDB_extension_data_aliases &&
            ext.data.u.aliases.aliases.len > 0) {
            ctx->aliases = ext.data.u.aliases;
            break;
        }
        free_HDB_extension(&ext);
    }

    if (ctx->tl != NULL && ctx->aliases.aliases.len > 0) {
        *out = &ctx->aliases.aliases.val[ctx->alias_idx++];
        return 0;
    }

    ctx->done = 1;
    return 0;
}

static kadm5_ret_t
check_aliases(kadm5_server_context *contextp,
              kadm5_principal_ent_rec *add_princ,
              kadm5_principal_ent_rec *del_princ)
{
    kadm5_ret_t ret;
    struct iter_aliases_ctx iter;
    struct iter_aliases_ctx iter_del;
    krb5_principal new_name, old_name;
    int match;

    /*
     * Yeah, this is O(N^2).  Gathering and sorting all the aliases
     * would be a bit of a pain; if we ever have principals with enough
     * aliases for this to be a problem, we can fix it then.
     */
    for (iter.done = -1; iter.done != 1;) {
        match = 0;
        ret = iter_aliases(add_princ, &iter, &new_name);
        if (ret)
            return ret;
        if (iter.done == 1)
            break;
        for (iter_del.done = -1; iter_del.done != 1;) {
            ret = iter_aliases(del_princ, &iter_del, &old_name);
            if (ret)
                return ret;
            if (iter_del.done == 1)
                break;
            if (!krb5_principal_compare(contextp->context, new_name, old_name))
                continue;
            free_HDB_Ext_Aliases(&iter_del.aliases);
            match = 1;
            break;
        }
        if (match)
            continue;
        ret = _kadm5_acl_check_permission(contextp, KADM5_PRIV_ADD, new_name);
        if (ret) {
            free_HDB_Ext_Aliases(&iter.aliases);
            return ret;
        }
    }

    return 0;
}

static void
v5_loop (krb5_context contextp,
	 krb5_auth_context ac,
	 krb5_boolean initial,
	 void *kadm_handlep,
	 krb5_socket_t fd,
         int readonly)
{
    krb5_error_code ret;
    krb5_data in, out;

    for (;;) {
	doing_useful_work = 0;
	if(term_flag)
	    exit(0);
	ret = krb5_read_priv_message(contextp, ac, &fd, &in);
	if(ret == HEIM_ERR_EOF)
	    exit(0);
	if(ret)
	    krb5_err(contextp, 1, ret, "krb5_read_priv_message");
	doing_useful_work = 1;
        ret = kadmind_dispatch(kadm_handlep, initial, &in, ac, fd, &out,
                               readonly);
	if (ret)
	    krb5_err(contextp, 1, ret, "kadmind_dispatch");
	krb5_data_free(&in);
        if (out.length)
            ret = krb5_write_priv_message(contextp, ac, &fd, &out);
	krb5_data_free(&out);
	if(ret)
	    krb5_err(contextp, 1, ret, "krb5_write_priv_message");
    }
}

static krb5_boolean
match_appl_version(const void *data, const char *appl_version)
{
    unsigned minor;
    if(sscanf(appl_version, "KADM0.%u", &minor) != 1)
	return 0;
    /*XXX*/
    *(unsigned*)(intptr_t)data = minor;
    return 1;
}

static void
handle_v5(krb5_context contextp,
	  krb5_keytab keytab,
	  krb5_socket_t fd,
          int readonly)
{
    krb5_error_code ret;
    krb5_ticket *ticket;
    char *server_name;
    char *client;
    void *kadm_handlep;
    krb5_boolean initial;
    krb5_auth_context ac = NULL;
    unsigned kadm_version = 1;
    kadm5_config_params realm_params;

    ret = krb5_recvauth_match_version(contextp, &ac, &fd,
				      match_appl_version, &kadm_version,
				      NULL, KRB5_RECVAUTH_IGNORE_VERSION,
				      keytab, &ticket);
    if (ret) {
	krb5_err(contextp, 1, ret, "krb5_recvauth");
        return;
    }
    ret = krb5_unparse_name(contextp, ticket->server, &server_name);
    if (ret) {
	krb5_err(contextp, 1, ret, "krb5_unparse_name");
        krb5_free_ticket(contextp, ticket);
        return;
    }
    if (strncmp(server_name, KADM5_ADMIN_SERVICE,
                     strlen(KADM5_ADMIN_SERVICE)) != 0) {
        krb5_errx(contextp, 1, "ticket for strange principal (%s)", server_name);
        krb5_free_ticket(contextp, ticket);
        free(server_name);
        return;
    }
    free(server_name);

    memset(&realm_params, 0, sizeof(realm_params));

    if(kadm_version == 1) {
	krb5_data params;
	ret = krb5_read_priv_message(contextp, ac, &fd, &params);
	if (ret) {
	    krb5_err(contextp, 1, ret, "krb5_read_priv_message");
            krb5_free_ticket(contextp, ticket);
            return;
        }
	ret = _kadm5_unmarshal_params(contextp, &params, &realm_params);
        if (ret) {
	    krb5_err(contextp, 1, ret,
                      "Could not read or parse kadm5 parameters");
            krb5_free_ticket(contextp, ticket);
            return;
        }
    }

    initial = ticket->ticket.flags.initial;
    ret = krb5_unparse_name(contextp, ticket->client, &client);
    krb5_free_ticket(contextp, ticket);
    if (ret) {
	krb5_err(contextp, 1, ret, "krb5_unparse_name");
        return;
    }
    ret = kadm5_s_init_with_password_ctx(contextp,
					 client,
					 NULL,
					 KADM5_ADMIN_SERVICE,
					 &realm_params,
					 0, 0,
					 &kadm_handlep);
    if (ret) {
	krb5_err(contextp, 1, ret, "kadm5_init_with_password_ctx");
        return;
    }
    v5_loop(contextp, ac, initial, kadm_handlep, fd, readonly);
}

krb5_error_code
kadmind_loop(krb5_context contextp,
	     krb5_keytab keytab,
	     krb5_socket_t sock,
             int readonly)
{
    u_char buf[sizeof(KRB5_SENDAUTH_VERSION) + 4];
    ssize_t n;
    unsigned long len;

    n = krb5_net_read(contextp, &sock, buf, 4);
    if(n == 0)
	exit(0);
    if(n < 0)
	krb5_err(contextp, 1, errno, "read");
    _krb5_get_int(buf, &len, 4);

    if (len == sizeof(KRB5_SENDAUTH_VERSION)) {

	n = krb5_net_read(contextp, &sock, buf + 4, len);
	if (n < 0)
	    krb5_err (contextp, 1, errno, "reading sendauth version");
	if (n == 0)
	    krb5_errx (contextp, 1, "EOF reading sendauth version");

	if(memcmp(buf + 4, KRB5_SENDAUTH_VERSION, len) == 0) {
	    handle_v5(contextp, keytab, sock, readonly);
	    return 0;
	}
	len += 4;
    } else
	len = 4;

    handle_mit(contextp, buf, len, sock, readonly);

    return 0;
}

