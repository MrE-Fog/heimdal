include $(top_srcdir)/Makefile.am.common

AM_CPPFLAGS += $(INCLUDE_openssl_crypto)

lib_LTLIBRARIES = libhx509.la libhx509template.la
libhx509_la_LDFLAGS = -version-info 5:0:0
libhx509template_la_LDFLAGS = -version-info 5:0:0

BUILT_SOURCES =				\
	sel-gram.h			\
	hx509_err.c			\
	hx509_err.h

AM_YFLAGS = -d -o sel-gram.c

dist_libhx509_la_SOURCES = \
	ca.c \
	cert.c \
	char_map.h \
	cms.c \
	collector.c \
	crypto.c \
	crypto-ec.c \
	doxygen.c \
	error.c \
	env.c \
	file.c \
	hx509.h \
	hx_locl.h \
	sel.c \
	sel.h \
	sel-gram.y \
	sel-lex.l \
	keyset.c \
	ks_dir.c \
	ks_file.c \
	ks_mem.c \
	ks_null.c \
	ks_p11.c \
	ks_p12.c \
	ks_keychain.c \
	lock.c \
	name.c \
	peer.c \
	print.c \
	softp11.c \
	ref/pkcs11.h \
	req.c \
	revoke.c

dist_libhx509template_la_SOURCES = $(dist_libhx509_la_SOURCES)

sel-gram.h: sel-gram.c
sel-lex.c: sel-gram.h

libhx509_la_DEPENDENCIES = version-script.map
libhx509template_la_DEPENDENCIES = version-script.map

libhx509_la_LIBADD = \
	$(LIB_com_err) \
	$(LIB_hcrypto) \
	$(LIB_openssl_crypto) \
	$(top_builddir)/lib/asn1/libasn1.la \
	$(top_builddir)/lib/wind/libwind.la \
	$(top_builddir)/lib/base/libheimbase.la \
	$(LIBADD_roken) \
	$(LIB_dlopen)

libhx509template_la_LIBADD = \
	$(LIB_com_err) \
	$(LIB_hcrypto) \
	$(LIB_openssl_crypto) \
	$(top_builddir)/lib/asn1/libasn1template.la \
	$(top_builddir)/lib/wind/libwind.la \
	$(top_builddir)/lib/base/libheimbase.la \
	$(LIBADD_roken) \
	$(LIB_dlopen)

if FRAMEWORK_SECURITY
libhx509_la_LDFLAGS += -framework Security -framework CoreFoundation
libhx509template_la_LDFLAGS += -framework Security -framework CoreFoundation
endif

if versionscript
libhx509_la_LDFLAGS += $(LDFLAGS_VERSION_SCRIPT)$(srcdir)/version-script.map
libhx509template_la_LDFLAGS += $(LDFLAGS_VERSION_SCRIPT)$(srcdir)/version-script.map
endif
$(libhx509_la_OBJECTS): $(srcdir)/version-script.map $(nodist_include_HEADERS) $(priv_headers)
$(libhx509template_la_OBJECTS): $(srcdir)/version-script.map $(nodist_include_HEADERS) $(priv_headers)

nodist_libhx509_la_SOURCES = $(BUILT_SOURCES)
nodist_libhx509template_la_SOURCES = $(BUILT_SOURCES)

dist_include_HEADERS = hx509.h $(srcdir)/hx509-protos.h

noinst_HEADERS = $(srcdir)/hx509-private.h

nodist_include_HEADERS = hx509_err.h

ALL_OBJECTS  = $(libhx509_la_OBJECTS)
ALL_OBJECTS += $(hxtool_OBJECTS)

HX509_PROTOS = $(srcdir)/hx509-protos.h $(srcdir)/hx509-private.h

$(ALL_OBJECTS): $(HX509_PROTOS)

$(libhx509_la_OBJECTS): $(srcdir)/hx_locl.h
$(libhx509template_la_OBJECTS): $(srcdir)/hx_locl.h

$(srcdir)/hx509-protos.h: $(dist_libhx509_la_SOURCES)
	$(heim_verbose)cd $(srcdir) && perl ../../cf/make-proto.pl -R '^(_|^C)' -E HX509_LIB -q -P comment -o hx509-protos.h $(dist_libhx509_la_SOURCES) || rm -f hx509-protos.h

$(srcdir)/hx509-private.h: $(dist_libhx509_la_SOURCES)
	$(heim_verbose)cd $(srcdir) && perl ../../cf/make-proto.pl -q -P comment -p hx509-private.h $(dist_libhx509_la_SOURCES) || rm -f hx509-private.h

bin_PROGRAMS = hxtool

hxtool-commands.c hxtool-commands.h: hxtool-commands.in $(SLC)
	$(heim_verbose)$(SLC) $(srcdir)/hxtool-commands.in

dist_hxtool_SOURCES = hxtool.c
nodist_hxtool_SOURCES = hxtool-commands.c hxtool-commands.h

$(hxtool_OBJECTS): hxtool-commands.h $(nodist_include_HEADERS)

hxtool_LDADD = \
	libhx509template.la \
	$(top_builddir)/lib/asn1/libasn1.la \
	$(LIB_hcrypto) \
	$(LIB_roken) \
	$(top_builddir)/lib/sl/libsl.la

CLEANFILES = $(BUILT_SOURCES) sel-gram.c sel-lex.c \
	$(TESTS) \
	hxtool-commands.c hxtool-commands.h *.tmp \
	request.out \
	out.pem out2.pem \
	sd sd.pem \
	sd.data sd.data.out \
	ev.data ev.data.out \
	cert-null.pem cert-sub-ca2.pem \
	cert-ee.pem cert-ca.pem \
	cert-sub-ee.pem cert-sub-ca.pem \
	cert-proxy.der cert-ca.der cert-ee.der pkcs10-request.der \
	wca.pem wuser.pem wdc.pem wcrl.crl \
	random-data statfile crl.crl \
	test p11dbg.log pkcs11.cfg \
	test-rc-file.rc

clean-local:
	@echo "cleaning PKITS" ; rm -rf PKITS_data

#
# regression tests
#

check_SCRIPTS = $(SCRIPT_TESTS)
check_PROGRAMS = $(PROGRAM_TESTS) test_soft_pkcs11

LDADD = libhx509.la

test_soft_pkcs11_LDADD = libhx509.la $(top_builddir)/lib/asn1/libasn1.la

test_name_LDADD = libhx509.la $(LIB_roken) $(top_builddir)/lib/asn1/libasn1.la
test_expr_LDADD = libhx509.la $(LIB_roken) $(top_builddir)/lib/asn1/libasn1.la

TESTS = $(SCRIPT_TESTS) $(PROGRAM_TESTS)

PROGRAM_TESTS = 		\
	test_name		\
	test_expr

SCRIPT_TESTS = 			\
	test_ca			\
	test_cert		\
	test_chain		\
	test_cms		\
	test_crypto		\
	test_nist		\
	test_nist2		\
	test_pkcs11		\
	test_java_pkcs11	\
	test_nist_cert		\
	test_nist_pkcs12	\
	test_req		\
	test_windows		\
	test_query

do_subst = $(heim_verbose)sed -e 's,[@]srcdir[@],$(srcdir),g' \
	-e 's,[@]objdir[@],$(top_builddir)/lib/hx509,g' \
	-e 's,[@]egrep[@],$(EGREP),g'

test_ca: test_ca.in Makefile
	$(do_subst) < $(srcdir)/test_ca.in > test_ca.tmp
	$(heim_verbose)chmod +x test_ca.tmp
	mv test_ca.tmp test_ca

test_cert: test_cert.in Makefile
	$(do_subst) < $(srcdir)/test_cert.in > test_cert.tmp
	$(heim_verbose)chmod +x test_cert.tmp
	mv test_cert.tmp test_cert

test_chain: test_chain.in Makefile
	$(do_subst) < $(srcdir)/test_chain.in > test_chain.tmp
	$(heim_verbose)chmod +x test_chain.tmp
	mv test_chain.tmp test_chain

test_cms: test_cms.in Makefile
	$(do_subst) < $(srcdir)/test_cms.in > test_cms.tmp
	$(heim_verbose)chmod +x test_cms.tmp
	mv test_cms.tmp test_cms

test_crypto: test_crypto.in Makefile
	$(do_subst) < $(srcdir)/test_crypto.in > test_crypto.tmp
	$(heim_verbose)chmod +x test_crypto.tmp
	mv test_crypto.tmp test_crypto

test_nist: test_nist.in Makefile
	$(do_subst) < $(srcdir)/test_nist.in > test_nist.tmp
	$(heim_verbose)chmod +x test_nist.tmp
	mv test_nist.tmp test_nist

test_nist2: test_nist2.in Makefile
	$(do_subst) < $(srcdir)/test_nist2.in > test_nist2.tmp
	$(heim_verbose)chmod +x test_nist2.tmp
	mv test_nist2.tmp test_nist2

test_pkcs11: test_pkcs11.in Makefile
	$(do_subst) < $(srcdir)/test_pkcs11.in > test_pkcs11.tmp
	$(heim_verbose)chmod +x test_pkcs11.tmp
	mv test_pkcs11.tmp test_pkcs11

test_java_pkcs11: test_java_pkcs11.in Makefile
	$(do_subst) < $(srcdir)/test_java_pkcs11.in > test_java_pkcs11.tmp
	$(heim_verbose)chmod +x test_java_pkcs11.tmp
	mv test_java_pkcs11.tmp test_java_pkcs11

test_nist_cert: test_nist_cert.in Makefile
	$(do_subst) < $(srcdir)/test_nist_cert.in > test_nist_cert.tmp
	$(heim_verbose)chmod +x test_nist_cert.tmp
	mv test_nist_cert.tmp test_nist_cert

test_nist_pkcs12: test_nist_pkcs12.in Makefile
	$(do_subst) < $(srcdir)/test_nist_pkcs12.in > test_nist_pkcs12.tmp
	$(heim_verbose)chmod +x test_nist_pkcs12.tmp
	mv test_nist_pkcs12.tmp test_nist_pkcs12

test_req: test_req.in Makefile
	$(do_subst) < $(srcdir)/test_req.in > test_req.tmp
	$(heim_verbose)chmod +x test_req.tmp
	mv test_req.tmp test_req

test_windows: test_windows.in Makefile
	$(do_subst) < $(srcdir)/test_windows.in > test_windows.tmp
	$(heim_verbose)chmod +x test_windows.tmp
	mv test_windows.tmp test_windows

test_query: test_query.in Makefile
	$(do_subst) < $(srcdir)/test_query.in > test_query.tmp
	$(heim_verbose)chmod +x test_query.tmp
	mv test_query.tmp test_query

EXTRA_DIST = \
	NTMakefile \
	hxtool-version.rc \
	libhx509-exports.def \
	version-script.map \
	hx509_err.et \
	hxtool-commands.in \
	quote.py \
	test_ca.in \
	test_chain.in \
	test_cert.in \
	test_cms.in \
	test_crypto.in \
	test_nist.in \
	test_nist2.in \
	test_nist_cert.in \
	test_nist_pkcs12.in \
	test_pkcs11.in \
	test_java_pkcs11.in \
	test_query.in \
	test_req.in \
	test_windows.in \
	tst-crypto-available1 \
	tst-crypto-available2 \
	tst-crypto-available3 \
	tst-crypto-select \
	tst-crypto-select1 \
	tst-crypto-select2 \
	tst-crypto-select3 \
	tst-crypto-select4 \
	tst-crypto-select5 \
	tst-crypto-select6 \
	tst-crypto-select7 \
	data/PKITS_data.zip \
	data/eccurve.pem \
	data/https.crt \
	data/https.key \
	data/mkcert.sh \
	data/nist-result2 \
	data/n0ll.pem \
	data/secp256r1TestCA.cert.pem \
	data/secp256r1TestCA.key.pem \
	data/secp256r1TestCA.pem \
	data/secp256r2TestClient.cert.pem \
	data/secp256r2TestClient.key.pem \
	data/secp256r2TestClient.pem \
	data/secp256r2TestServer.cert.pem \
	data/secp256r2TestServer.key.pem \
	data/secp256r2TestServer.pem \
	data/bleichenbacher-bad.pem \
	data/bleichenbacher-good.pem \
	data/bleichenbacher-sf-pad-correct.pem \
	data/ca.crt \
	data/ca.key \
	data/crl1.crl \
	data/crl1.der \
	data/gen-req.sh \
	data/j.pem \
	data/kdc.crt \
	data/kdc.key \
	data/key.der \
	data/key2.der \
	data/nist-data \
	data/nist-data2 \
	data/no-proxy-test.crt \
	data/no-proxy-test.key \
	data/ocsp-req1.der \
	data/ocsp-req2.der \
	data/ocsp-resp1-2.der \
	data/ocsp-resp1-3.der \
	data/ocsp-resp1-ca.der \
	data/ocsp-resp1-keyhash.der \
	data/ocsp-resp1-ocsp-no-cert.der \
	data/ocsp-resp1-ocsp.der \
	data/ocsp-resp1.der \
	data/ocsp-resp2.der \
	data/ocsp-responder.crt \
	data/ocsp-responder.key \
	data/openssl.1.0.cnf \
	data/openssl.1.1.cnf \
	data/pkinit-proxy-chain.crt \
	data/pkinit-proxy.crt \
	data/pkinit-proxy.key \
	data/pkinit-pw.key \
	data/pkinit.crt \
	data/pkinit.key \
	data/pkinit-ec.crt \
	data/pkinit-ec.key \
	data/proxy-level-test.crt \
	data/proxy-level-test.key \
	data/proxy-test.crt \
	data/proxy-test.key \
	data/proxy10-child-test.crt \
	data/proxy10-child-test.key \
	data/proxy10-child-child-test.crt \
	data/proxy10-child-child-test.key \
	data/proxy10-test.crt \
	data/proxy10-test.key \
	data/revoke.crt \
	data/revoke.key \
	data/sf-class2-root.pem \
	data/static-file \
	data/sub-ca.crt \
	data/sub-ca.key \
	data/sub-cert.crt \
	data/sub-cert.key \
	data/sub-cert.p12 \
	data/test-ds-only.crt \
	data/test-ds-only.key \
	data/test-enveloped-aes-128 \
	data/test-enveloped-aes-256 \
	data/test-enveloped-des \
	data/test-enveloped-des-ede3 \
	data/test-enveloped-rc2-128 \
	data/test-enveloped-rc2-40 \
	data/test-enveloped-rc2-64 \
	data/test-ke-only.crt \
	data/test-ke-only.key \
	data/test-nopw.p12 \
	data/test-pw.key \
	data/test-signed-data \
	data/test-signed-data-noattr \
	data/test-signed-data-noattr-nocerts \
	data/test-signed-sha-1 \
	data/test-signed-sha-256 \
	data/test-signed-sha-512 \
	data/test.combined.crt \
	data/test.crt \
	data/test.key \
	data/test.p12 \
	data/win-u16-in-printablestring.der \
	data/yutaka-pad-broken-ca.pem \
	data/yutaka-pad-broken-cert.pem \
	data/yutaka-pad-ok-ca.pem \
	data/yutaka-pad-ok-cert.pem \
	data/yutaka-pad.key
