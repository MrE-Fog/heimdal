#include "admin_locl.h"

RCSID("$Id$");

static void
doit(char *principal, int mod)
{
    HDB *db;
    int err;
    hdb_entry ent;
    hdb_entry def;
    int32_t tmp;
    char buf[1024];
    
    krb5_parse_name(context, principal, &ent.principal);
    
    if((err = hdb_open(context, &db, database, O_RDWR, 0600))){
	fprintf(stderr, "hdb_open: %s\n", krb5_get_err_text(context, err));
	return;
    }
    
    err = db->fetch(context, db, &ent);
    
    switch(err){
    case KRB5_HDB_NOENTRY:
	if(mod){
	    fprintf(stderr, "Entry not found in database\n");
	    return;
	}else{
	    krb5_realm *realm;
	    
	    realm = krb5_princ_realm(context, ent.principal);
#ifdef USE_ASN1_PRINCIPAL
	    krb5_build_principal(context, &def.principal, 
				 strlen(*realm),
				 *realm,
				 "default",
				 NULL);
#else
	    krb5_build_principal(context, &def.principal, 
				 realm->length,
				 realm->data,
				 "default",
				 NULL);
#endif
	    if(db->fetch(context, db, &def)){
		/* XXX */
	    }
	    ent.kvno = 0;
	    ent.max_life = def.max_life;
	    ent.max_renew = def.max_renew;
	    ent.expires = def.expires;
	    if(ent.expires)
		ent.expires += time(NULL);
	    break;
	}
    case 0:
	if(!mod){
	    warnx("Principal exists");
	    return;
	}
	break;
    default:
	errx(1, "dbget: %s", krb5_get_err_text(context, err));
    }
    printf("Max ticket life [%d]: ", ent.max_life);
    fgets(buf, sizeof(buf), stdin);
    if(sscanf(buf, "%d", &tmp) == 1)
	ent.max_life = tmp;
    printf("Max renewable ticket [%d]: ", ent.max_renew);
    fgets(buf, sizeof(buf), stdin);
    if(sscanf(buf, "%d", &tmp) == 1)
	ent.max_renew = tmp;
    while(mod){
	fprintf(stderr, "Change password? (y/n) ");
	fgets(buf, sizeof(buf), stdin);
	if(buf[0] == 'n' || buf[0] == 'y')
	    break;
	else {
	    fprintf(stderr, "Please answer yes or no.\n");
	    continue;
	}
    }
    if(mod == 0 || buf[0] == 'y'){
	krb5_data salt;
	des_read_pw_string(buf, sizeof(buf), "Password:", 1);
	memset(&salt, 0, sizeof(salt));
	krb5_get_salt(ent.principal, &salt);
	memset(&ent.keyblock, 0, sizeof(ent.keyblock));
	krb5_string_to_key(buf, &salt, &ent.keyblock);
	ent.kvno++;
    }
    ent.last_change = time(NULL);
    {
	krb5_realm *realm = krb5_princ_realm(context, ent.principal);
	
#ifdef USE_ASN1_PRINCIPAL
	krb5_build_principal(context, &ent.changed_by,
			     strlen(*realm),
			     *realm,
			     "kadmin",
			     NULL);
#else
	krb5_build_principal(context, &ent.changed_by, 
			     krb5_princ_realm(context, ent.principal)->length,
			     krb5_princ_realm(context, ent.principal)->data,
			     "kadmin",
			     NULL);
#endif
    }
    err = db->store(context, db, &ent);
    if(err == -1){
	perror("dbput");
	exit(1);
    }
    hdb_free_entry(context, &ent);
    db->close(context, db);
}


void
add_new_key(int argc, char **argv)
{
    if(argc != 2){
	fprintf(stderr, "Usage: add_new_key principal\n");
	return;
    }

    doit(argv[1], 0);
}

void
mod_entry(int argc, char **argv)
{
    if(argc != 2){
	fprintf(stderr, "Usage: mod_entry principal\n");
	return;
    }

    doit(argv[1], 1);
}
