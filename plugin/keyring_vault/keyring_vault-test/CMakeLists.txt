INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/plugin/keyring_vault
  ${CMAKE_SOURCE_DIR}/unittest/gunit/keyring_vault)

MYSQL_ADD_EXECUTABLE(keyring_vault-test
               ./keyring_vault-test.cc
               ${CMAKE_SOURCE_DIR}/plugin/keyring/common/keyring_key.cc
               ${CMAKE_SOURCE_DIR}/plugin/keyring/common/keys_container.cc
               ${CMAKE_SOURCE_DIR}/plugin/keyring/common/keyring_impl.cc
               ../vault_io.cc
               ../vault_key.cc
               ../vault_base64.cc
               ../vault_parser.cc
               ../vault_keys_list.cc
               ../vault_keys_container.cc
               ../vault_curl.cc
               ../vault_credentials_parser.cc
               ../vault_credentials.cc
               ADD_TEST)

TARGET_LINK_LIBRARIES(keyring_vault-test strings regex mysys sql)
TARGET_LINK_LIBRARIES(keyring_vault-test binlog rpl master slave)
TARGET_LINK_LIBRARIES(keyring_vault-test ${CURL_LIBRARY} ${SSL_LIBRARIES})

ADD_TEST(keyring_vault-test keyring_vault-test)
