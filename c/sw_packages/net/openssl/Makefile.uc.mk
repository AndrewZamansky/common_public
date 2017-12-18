
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_OPENSSL)

ifeq ($(strip $(CONFIG_INCLUDE_OPENSSL)),y)

    CURR_OPENSSL_COMPONENT_LOCATION := $(patsubst %/Makefile.uc.mk,%,$(realpath $(filter %openssl/Makefile.uc.mk,$(MAKEFILE_LIST))))
    OPENSSL_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/openssl
    ifeq ("$(wildcard $(OPENSSL_PATH))","")
        $(info   )
        $(info --- OpenSSL path $(OPENSSL_PATH) dont exists )
        $(info --- get repo from andew zamansky or from https://github.com/openssl/openssl  )
        $(info --- make sure that .git directory is located in $(OPENSSL_PATH)/  after unpacking   )
        $(error )
    endif

    #test if current commit and branch of openssl git is the same as required by application
    CURR_GIT_REPO_DIR :=$(OPENSSL_PATH)
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_OPENSSL_GIT_COMMIT_HASH
    CURR_GIT_BUNDLE :=$(CURR_OPENSSL_COMPONENT_LOCATION)/openssl.bundle
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk
    
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_OPENSSL_COMPONENT_LOCATION)/include)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_OPENSSL_COMPONENT_LOCATION)/include/openssl)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(OPENSSL_PATH)/include)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(OPENSSL_PATH))
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(OPENSSL_PATH)/crypto/modes)
    
    ifneq ($(strip $(CONFIG_OPENSSL_MD5)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , OPENSSL_NO_MD5 )
    endif
    ifneq ($(strip $(CONFIG_OPENSSL_DES)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , OPENSSL_NO_DES )
    endif
    ifneq ($(strip $(CONFIG_OPENSSL_RSA)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , OPENSSL_NO_RSA )
    endif
    ifneq ($(strip $(CONFIG_OPENSSL_DSA)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , OPENSSL_NO_DSA )
    endif
    ifneq ($(strip $(CONFIG_OPENSSL_DH)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , OPENSSL_NO_DH )
    endif
    ifneq ($(strip $(CONFIG_OPENSSL_SOCKET)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , OPENSSL_NO_SOCK )
    endif
    
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_IDEA )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_AES )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_CAMELLIA )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_SEED )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_BF )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_CAST )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_RC2 )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_RC4 )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_RC5 )

        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_MD2 )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_MD4 )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_SHA )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_RIPEMD )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_MDC2 )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_MULTIBLOCK )


      #  DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_EC )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_ECDSA )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_ECDH )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_TLS1_3 )

       # DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_SOCK    )   #   No socket code.
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_SSL2 )     #    No SSLv2.
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_SSL3 )     #    No SSLv3.
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_ERR  )     #    No error strings.
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_KRB5   )   #    No Kerberos v5.
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_ENGINE )   #    No dynamic engines.
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_HW   )    #     No support for external hardware.

        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_TLSEXT )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_CMS )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_JPAKE )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_CAPIENG )

        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_WHRLPOOL )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_STORE )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_SHA0 )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_RFC3779 )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_GMP )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_FIPS )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_ASM )
        
        
        #DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,_WINSOCKAPI_)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,NO_WINDOWS_BRAINDEATH )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_CRYPTO_MDEBUG_BACKTRACE )
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,OPENSSL_NO_CRYPTO_MDEBUG )

endif

DEFINES = _WINSOCKAPI_


#ASMFLAGS =

INCLUDE_DIR :=
INCLUDE_DIR +=  $(OPENSSL_PATH)/crypto/include

SRC := cversion.c
SRC += mem.c
SRC += o_str.c
SRC += o_time.c
SRC += mem_clr.c
SRC += cryptlib.c
SRC += ex_data.c
SRC += mem_sec.c
SRC += init.c
SRC += o_fopen.c
SRC += mem_dbg.c
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)    
    SRC += threads_win.c
endif
SRC += o_fips.c
SRC += o_dir.c
VPATH += | $(OPENSSL_PATH)/crypto

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)    
    SRC += async_win.c
endif
VPATH += | $(OPENSSL_PATH)/crypto/async/arch

SRC += stack.c
VPATH += | $(OPENSSL_PATH)/crypto/stack

SRC += ocb128.c
SRC += cbc128.c
SRC += ctr128.c
SRC += ofb128.c
SRC += cfb128.c
SRC += gcm128.c
SRC += ccm128.c
SRC += xts128.c
SRC += wrap128.c
VPATH += | $(OPENSSL_PATH)/crypto/modes

SRC += dso_lib.c
SRC += dso_openssl.c
VPATH += | $(OPENSSL_PATH)/crypto/dso

SRC += chacha_enc.c
VPATH += | $(OPENSSL_PATH)/crypto/chacha

SRC += bio_lib.c
SRC += bss_mem.c
SRC += b_print.c
SRC += b_dump.c
SRC += bss_file.c
SRC += b_sock.c
SRC += b_addr.c
SRC += bio_meth.c
SRC += bss_conn.c
SRC += bf_buff.c
SRC += bio_meth.c
SRC += bss_file.c
SRC += bss_sock.c
SRC += b_sock2.c
SRC += bss_null.c
SRC += bio_md.c
VPATH += | $(OPENSSL_PATH)/crypto/bio

SRC += bn_print.c
SRC += bn_mont.c
SRC += bn_shift.c
SRC += bn_asm.c
SRC += bn_ctx.c
SRC += bn_mul.c
SRC += bn_sqr.c
SRC += bn_div.c
SRC += bn_gcd.c
SRC += bn_add.c
SRC += bn_mod.c
SRC += bn_rand.c
SRC += bn_exp.c
SRC += bn_recp.c
SRC += bn_prime.c
SRC += bn_exp2.c
SRC += bn_dh.c
SRC += bn_blind.c
SRC += bn_intern.c
SRC += bn_nist.c
SRC += bn_lib.c
SRC += bn_word.c
SRC += bn_const.c
SRC += bn_gf2m.c
SRC += bn_kron.c
SRC += bn_sqrt.c
SRC += bn_srp.c
VPATH += | $(OPENSSL_PATH)/crypto/bn

SRC += txt_db.c
VPATH += | $(OPENSSL_PATH)/crypto/txt_db

SRC += buffer.c
VPATH += | $(OPENSSL_PATH)/crypto/buffer

SRC += asn1_gen.c
SRC += asn1_lib.c
SRC += a_object.c
SRC += a_strex.c
SRC += a_time.c
SRC += a_print.c
SRC += a_utf8.c
SRC += asn1_par.c
SRC += a_mbstr.c
SRC += tasn_typ.c
SRC += tasn_new.c
SRC += tasn_fre.c
SRC += a_utctm.c
SRC += tasn_dec.c
SRC += a_bitstr.c
SRC += a_gentm.c
SRC += tasn_enc.c
SRC += asn_moid.c
SRC += asn_mstbl.c
SRC += a_strnid.c
SRC += x_bignum.c
SRC += x_long.c
SRC += a_dup.c
SRC += a_type.c
SRC += asn_pack.c
SRC += evp_asn1.c
SRC += a_octet.c
SRC += ameth_lib.c
SRC += x_attrib.c
SRC += rsa_asn1.c
SRC += a_int.c
SRC += t_pkey.c
SRC += x_algor.c
SRC += p8_pkey.c
SRC += p5_scrypt.c
SRC += p5_pbe.c
SRC += p5_pbev2.c
SRC += a_d2i_fp.c
SRC += nsseq.c
SRC += x_sig.c
SRC += a_i2d_fp.c
SRC += d2i_pr.c
SRC += x_pkey.c
SRC += i2d_pr.c
SRC += tasn_prn.c
SRC += f_int.c
SRC += a_sign.c
SRC += tasn_utl.c
SRC += f_string.c
SRC += x_spki.c
SRC += a_digest.c
SRC += a_verify.c
SRC += x_val.c
VPATH += | $(OPENSSL_PATH)/crypto/asn1

SRC += srp_vfy.c
SRC += srp_lib.c
VPATH += | $(OPENSSL_PATH)/crypto/srp

SRC += cm_ameth.c
SRC += cm_pmeth.c
VPATH += | $(OPENSSL_PATH)/crypto/cmac

SRC += pk7_lib.c
SRC += pk7_doit.c
SRC += pk7_asn1.c
SRC += pk7_attr.c
VPATH += | $(OPENSSL_PATH)/crypto/pkcs7

SRC += p_lib.c
SRC += evp_cnf.c
SRC += c_allc.c
SRC += c_alld.c
SRC += names.c
SRC += e_des.c
SRC += e_des3.c
SRC += e_aes.c
SRC += e_xcbc_d.c
SRC += e_aes_cbc_hmac_sha1.c
SRC += m_sha1.c
SRC += e_chacha20_poly1305.c
SRC += m_md5.c
SRC += m_md5_sha1.c
SRC += evp_lib.c
SRC += e_aes_cbc_hmac_sha256.c
SRC += m_ripemd.c
SRC += m_wp.c
SRC += evp_enc.c
SRC += evp_pbe.c
SRC += p5_crpt.c
SRC += p5_crpt2.c
SRC += digest.c
SRC += pmeth_lib.c
SRC += scrypt.c
SRC += evp_pkey.c
SRC += encode.c
SRC += evp_key.c
SRC += pmeth_gn.c
SRC += p_sign.c
SRC += p_verify.c
SRC += m_sigver.c
SRC += bio_enc.c
SRC += pmeth_fn.c
SRC += e_null.c
VPATH += | $(OPENSSL_PATH)/crypto/evp

SRC += cmac.c
VPATH += | $(OPENSSL_PATH)/crypto/cmac

SRC += p12_crpt.c
SRC += p12_key.c
SRC += p12_utl.c
SRC += p12_asn.c
SRC += p12_p8d.c
SRC += p12_decr.c
SRC += p12_p8e.c
VPATH += | $(OPENSSL_PATH)/crypto/pkcs12

SRC += aes_x86core.c
SRC += aes_cbc.c
VPATH += | $(OPENSSL_PATH)/crypto/aes

SRC += m_blake2b.c
SRC += m_blake2s.c
SRC += blake2b.c
SRC += blake2s.c
VPATH += | $(OPENSSL_PATH)/crypto/blake2

SRC += poly1305.c
SRC += poly1305_ameth.c
SRC += poly1305_pmeth.c
VPATH += | $(OPENSSL_PATH)/crypto/poly1305

SRC += rmd_dgst.c
VPATH += | $(OPENSSL_PATH)/crypto/ripemd

SRC += hm_ameth.c
SRC += hmac.c
SRC += hm_pmeth.c
VPATH += | $(OPENSSL_PATH)/crypto/hmac

SRC += wp_dgst.c
SRC += wp_block.c
VPATH += | $(OPENSSL_PATH)/crypto/whrlpool

SRC += lhash.c
VPATH += | $(OPENSSL_PATH)/crypto/lhash

SRC += obj_lib.c
SRC += obj_dat.c
SRC += o_names.c
SRC += obj_xref.c
VPATH += | $(OPENSSL_PATH)/crypto/objects

SRC += sha256.c
SRC += sha512.c
SRC += sha1_one.c
SRC += sha1dgst.c
VPATH += | $(OPENSSL_PATH)/crypto/sha

SRC += x509_lu.c
SRC += by_file.c
SRC += x509_txt.c
SRC += t_x509.c
SRC += x_x509.c
SRC += x_pubkey.c
SRC += x509_set.c
SRC += x509_cmp.c
SRC += x509name.c
SRC += x509_ext.c
SRC += x509_v3.c
SRC += x_name.c
SRC += x_all.c
SRC += x509_def.c
SRC += x_crl.c
SRC += x_info.c
SRC += x509_att.c
SRC += x_req.c
SRC += x509cset.c
SRC += x509_vfy.c
SRC += x509_vpm.c
SRC += by_dir.c
SRC += x509_d2.c
SRC += x_exten.c
SRC += x509type.c
SRC += x_x509a.c
SRC += x509_req.c
SRC += x509_obj.c
SRC += x509rset.c
VPATH += | $(OPENSSL_PATH)/crypto/x509

SRC += ct_policy.c
SRC += ct_sct.c
SRC += ct_oct.c
SRC += ct_log.c
SRC += ct_sct_ctx.c
SRC += ct_vfy.c
SRC += ct_b64.c
SRC += ct_x509v3.c
SRC += ct_prn.c
VPATH += | $(OPENSSL_PATH)/crypto/ct

SRC += v3_prn.c
SRC += v3_genn.c
SRC += v3_purp.c
SRC += v3_conf.c
SRC += v3_utl.c
SRC += v3_info.c
SRC += v3_lib.c
SRC += v3_alt.c
SRC += v3_bcons.c
SRC += v3_bitst.c
SRC += v3_sxnet.c
SRC += v3_pku.c
SRC += v3_extku.c
SRC += v3_ia5.c
SRC += v3_skey.c
SRC += v3_akey.c
SRC += v3_int.c
SRC += v3_enum.c
SRC += v3_cpols.c
SRC += v3_crld.c
SRC += v3_akeya.c
SRC += v3_pci.c
SRC += v3_pmaps.c
SRC += v3_pcons.c
SRC += v3_ncons.c
SRC += v3_tlsf.c
SRC += v3_pcia.c
SRC += pcy_tree.c
SRC += x509_trs.c
SRC += pcy_cache.c
SRC += pcy_data.c
SRC += pcy_map.c
SRC += pcy_lib.c
SRC += pcy_node.c
VPATH += | $(OPENSSL_PATH)/crypto/x509v3

SRC += err.c
SRC += err_prn.c
VPATH += | $(OPENSSL_PATH)/crypto/err

SRC += pem_x509.c
SRC += pem_info.c
SRC += pem_xaux.c
SRC += pem_all.c
SRC += pem_lib.c
SRC += pem_pkey.c
SRC += pem_oth.c
SRC += pem_pk8.c
VPATH += | $(OPENSSL_PATH)/crypto/pem

SRC += ssl_lib.c
SRC += ssl_rsa.c
SRC += bio_ssl.c
SRC += ssl_sess.c
SRC += methods.c
SRC += s3_lib.c
SRC += s3_msg.c
SRC += t1_lib.c
SRC += d1_lib.c
SRC += t1_enc.c
SRC += packet.c
SRC += pqueue.c
SRC += ssl_ciph.c
SRC += d1_msg.c
SRC += s3_enc.c
SRC += tls_srp.c
SRC += ssl_cert.c
SRC += tls13_enc.c
SRC += s3_cbc.c
SRC += t1_ext.c
SRC += ssl_init.c
SRC += ssl_mcnf.c
SRC += ssl_asn1.c
SRC += ssl_conf.c
SRC += d1_srtp.c
VPATH += | $(OPENSSL_PATH)/ssl

SRC += ec_ameth.c
SRC += ecx_meth.c
SRC += ec_key.c
SRC += ec_lib.c
SRC += eck_prn.c
SRC += ec_asn1.c
SRC += ec_curve.c
SRC += ec_cvt.c
SRC += ec_oct.c
SRC += ec_kmeth.c
SRC += ec_mult.c
SRC += ec_print.c
SRC += curve25519.c
SRC += ecp_mont.c
SRC += ecp_nist.c
SRC += ec2_smpl.c
SRC += ecdh_ossl.c
SRC += ecdsa_ossl.c
SRC += ecp_oct.c
SRC += ec2_oct.c
SRC += ec_pmeth.c
SRC += ecdh_kdf.c
SRC += ecdsa_sign.c
SRC += ecdsa_vrf.c
SRC += ecp_smpl.c
SRC += ec2_mult.c
VPATH += | $(OPENSSL_PATH)/crypto/ec

SRC += ui_lib.c
SRC += ui_openssl.c
VPATH += | $(OPENSSL_PATH)/crypto/ui

SRC += ocsp_asn.c
SRC += ocsp_cl.c
SRC += ocsp_ext.c
SRC += ocsp_lib.c
SRC += v3_ocsp.c
SRC += ocsp_ht.c
VPATH += | $(OPENSSL_PATH)/crypto/ocsp

SRC += statem.c
SRC += statem_dtls.c
SRC += statem_srvr.c
SRC += statem_lib.c
SRC += statem_clnt.c
SRC += extensions.c
SRC += extensions_srvr.c
SRC += extensions_clnt.c
VPATH += | $(OPENSSL_PATH)/ssl/statem

SRC += rec_layer_s3.c
SRC += rec_layer_d1.c
SRC += ssl3_record.c
SRC += ssl3_buffer.c
SRC += dtls1_bitmap.c
SRC += ssl3_record_tls13.c
VPATH += | $(OPENSSL_PATH)/ssl/record

SRC += rand_lib.c
SRC += randfile.c
SRC += md_rand.c
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)    
    SRC += rand_win.c
endif
VPATH += | $(OPENSSL_PATH)/crypto/rand

SRC += c_zlib.c
SRC += comp_lib.c
VPATH += | $(OPENSSL_PATH)/crypto/comp

SRC += async.c
SRC += async_wait.c
VPATH += | $(OPENSSL_PATH)/crypto/async

SRC += conf_mod.c
SRC += conf_mall.c
SRC += conf_lib.c
SRC += conf_def.c
SRC += conf_api.c
SRC += conf_sap.c
VPATH += | $(OPENSSL_PATH)/crypto/conf

SRC += tls1_prf.c
SRC += hkdf.c
VPATH += | $(OPENSSL_PATH)/crypto/kdf

ifeq ($(strip $(CONFIG_OPENSSL_RSA)),y)
    SRC += rsa_lib.c
    SRC += rsa_ameth.c
    SRC += rsa_pmeth.c
    SRC += rsa_crpt.c
    SRC += rsa_ossl.c
    SRC += rsa_crpt.c
    SRC += rsa_gen.c
    SRC += rsa_sign.c
    SRC += rsa_saos.c
    SRC += rsa_oaep.c
    SRC += rsa_x931.c
    SRC += rsa_pss.c
    SRC += rsa_pk1.c
    SRC += rsa_ssl.c
    SRC += rsa_none.c
    VPATH += | $(OPENSSL_PATH)/crypto/rsa
endif

ifeq ($(strip $(CONFIG_OPENSSL_DSA)),y)
    SRC += dsa_lib.c
    SRC += dsa_ossl.c
    SRC += dso_dl.c
    SRC += dsa_asn1.c
    SRC += dso_dlfcn.c
    SRC += dsa_sign.c
    SRC += dsa_vrf.c
    SRC += dsa_ameth.c
    SRC += dsa_pmeth.c
    SRC += dsa_gen.c
    SRC += dsa_key.c
    VPATH += | $(OPENSSL_PATH)/crypto/dsa
endif

ifeq ($(strip $(CONFIG_OPENSSL_DH)),y)
    SRC += dh_lib.c
    SRC += dh_key.c
    SRC += dso_dl.c
    SRC += dh_check.c
    SRC += dh_ameth.c
    SRC += dh_asn1.c
    SRC += dh_pmeth.c
    SRC += dh_gen.c
    SRC += dh_rfc5114.c
    VPATH += | $(OPENSSL_PATH)/crypto/dh
endif

ifeq ($(strip $(CONFIG_OPENSSL_MD5)),y)
    SRC += md5_dgst.c
    VPATH += | $(OPENSSL_PATH)/crypto/md5
endif

ifeq ($(strip $(CONFIG_OPENSSL_DES)),y)
    SRC += set_key.c
    SRC += ecb_enc.c
    SRC += des_enc.c
    SRC += cfb_enc.c
    SRC += cfb64enc.c
    SRC += ofb64enc.c
    SRC += ecb3_enc.c
    SRC += cfb64ede.c
    SRC += ofb64ede.c
    SRC += xcbc_enc.c
    VPATH += | $(OPENSSL_PATH)/crypto/des
endif


include $(COMMON_CC)
