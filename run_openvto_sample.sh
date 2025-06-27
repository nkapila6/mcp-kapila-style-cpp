#!/bin/bash
/Users/nkapila6/Code/mcp-kapila-style-cpp/build/bin/mcp_openvto \
  --user user \
  --pass pass \
  --hostname *.cloud.couchbase.com \
  --port 18094 \
  --bucket bucket_name \
  --scope scope_name \
  --search-index search_index_name \
  --search-field db_key_to_search_name \
  --api-key replicate_api_key \
  --version replicate_model_version \
  --csv-filepath csv_path_for_local_search \
  --img-link img_txt_file_link \
  --is-img-path true \
  --verbose true