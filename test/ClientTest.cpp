#include "CasperClient.h"
#include "Types/GlobalStateKey.cpp"
#include "Types/PublicKey.h"
#include "Utils/CryptoUtil.h"
#include "acutest.h"

/**
 * @brief Check the "info_get_peers" rpc function. Size of the resulting array
 * shouldn't be 0.
 *
 */
void get_peers_test(void) {
  Casper::Client client(CASPER_TEST_ADDRESS);
  TEST_ASSERT(client.GetNodePeers().peers.size() != 0);
}

/**
 * @brief Check the "get_state_root_hash" rpc function with a variable. Compare
 * the result with an empty string.
 *
 */
void get_state_root_hash_block_height_test(void) {
  Casper::Client client(CASPER_TEST_ADDRESS);
  uint64_t block_height = 10;
  TEST_ASSERT(client.GetStateRootHash(block_height).state_root_hash != "");
}

/**
 * @brief Check the "get_state_root_hash" rpc function without a variable.
 * Compare the result with an empty string.
 *
 */
void get_state_root_hash_test() {
  Casper::Client client(CASPER_TEST_ADDRESS);
  TEST_ASSERT(client.GetStateRootHash().state_root_hash != "");
}

/**
 * @brief Check the "info_get_deploy" rpc function
 *
 */
void get_deploy_test() {
  Casper::Client client(CASPER_TEST_ADDRESS);
  // Casper::GetDeployInfoResult result = client.GetDeployInfo();

  // TEST_ASSERT(result.api_version != "");
}

/**
 * @brief Check the "info_get_status" rpc function. Check the result
 * variables.
 *
 */
void get_status_info_test() {
  Casper::Client client(CASPER_TEST_ADDRESS);
  Casper::GetStatusResult result = client.GetStatusInfo();

  TEST_ASSERT(result.api_version != "");
  TEST_ASSERT(result.chainspec_name != "");
  TEST_ASSERT(result.starting_state_root_hash != "");
  TEST_ASSERT(result.peers.size() != 0);
  TEST_ASSERT(result.build_version != "");
  TEST_ASSERT(result.uptime != "");
}

/**
 * @brief Check the "chain_get_block_transfers" rpc function.
 *
 */
void get_block_transfers_test() {
  Casper::Client client(CASPER_TEST_ADDRESS);
  Casper::GetBlockTransfersResult result = client.GetBlockTransfers(
      "35f86b6ab5e13b823daee5d23f3373f6b35048e0b0ea993adfadc5ba8ee7aae5");

  TEST_ASSERT(result.api_version != "");

  TEST_ASSERT(result.block_hash.has_value());
  TEST_ASSERT(result.block_hash.value() != "");

  TEST_ASSERT(result.transfers.has_value());
  TEST_ASSERT(result.transfers.value().size() != 0);
  TEST_ASSERT(result.transfers.value()[0].deploy_hash != "");
  TEST_ASSERT(result.transfers.value()[0].source.ToString() != "");
  TEST_ASSERT(result.transfers.value()[0].target.ToString() != "");
  TEST_ASSERT(result.transfers.value()[0].amount != 0);
  TEST_ASSERT(result.transfers.value()[0].gas == 0);
}

/**
 * @brief Check the "chain_get_block" rpc function
 *
 */
void get_block_test() {
  Casper::Client client(CASPER_TEST_ADDRESS);
  std::string block_hash =
      "acc4646f35cc1d59b24381547a4d2dc1c992a202b6165f3bf68d3f23c2b93330";

  Casper::GetBlockResult blockResult = client.GetBlock(block_hash);

  TEST_ASSERT(blockResult.api_version != "");

  TEST_ASSERT(blockResult.block.has_value());

  TEST_ASSERT(blockResult.block.value().hash != "");

  // block header
  TEST_ASSERT(blockResult.block.value().header.parent_hash != "");
  TEST_ASSERT(blockResult.block.value().header.state_root_hash != "");
  TEST_ASSERT(blockResult.block.value().header.body_hash != "");
  TEST_ASSERT(blockResult.block.value().header.accumulated_seed != "");
  TEST_ASSERT(blockResult.block.value().header.timestamp != "");
  TEST_ASSERT(blockResult.block.value().header.era_id != 0);
  TEST_ASSERT(blockResult.block.value().header.height != 0);
  TEST_ASSERT(blockResult.block.value().header.protocol_version != "");

  // block body
  TEST_ASSERT(blockResult.block.value().body.deploy_hashes.size() >= 0);
  TEST_ASSERT(blockResult.block.value().body.proposer.ToString() != "");
  TEST_ASSERT(blockResult.block.value().body.transfer_hashes.size() >= 0);

  // block proofs
  TEST_ASSERT(blockResult.block.value().proofs.size() > 0);
  TEST_ASSERT(blockResult.block.value().proofs[0].public_key.ToString() != "");
  TEST_ASSERT(blockResult.block.value().proofs[0].signature.ToString() != "");
}

/**
 * @brief Check the "chain_get_era_info_by_switch_block" rpc function
 *
 */
void get_era_info_by_switch_block_test() {
  Casper::Client client(CASPER_TEST_ADDRESS);
  Casper::GetEraInfoResult result = client.GetEraInfoBySwitchBlock(
      "d2077716e5b8796723c5720237239720f54e6ada54e3357f2c4896f2a51a6d8f");

  TEST_ASSERT(result.api_version != "");
  TEST_ASSERT(result.era_summary.has_value());
  TEST_ASSERT(result.era_summary.value().era_id != 0);
  TEST_ASSERT(result.era_summary.value().block_hash != "");
  TEST_ASSERT(result.era_summary.value().merkle_proof != "");
  TEST_ASSERT(result.era_summary.value().state_root_hash != "");
  TEST_ASSERT(result.era_summary.value().stored_value.era_info.has_value());
  TEST_ASSERT(result.era_summary.value()
                  .stored_value.era_info.value()
                  .seigniorage_allocations.size() > 0);

  int validator_cnt = 0;
  int delegator_cnt = 0;

  for (int i = 0; i < result.era_summary.value()
                          .stored_value.era_info.value()
                          .seigniorage_allocations.size();
       i++) {
    if (delegator_cnt == 1 && validator_cnt == 1) {
      break;
    }

    bool is_delegator = result.era_summary.value()
                            .stored_value.era_info.value()
                            .seigniorage_allocations[i]
                            .is_delegator;

    if (is_delegator == true && delegator_cnt == 0) {
      delegator_cnt++;
      TEST_ASSERT(result.era_summary.value()
                      .stored_value.era_info.value()
                      .seigniorage_allocations[i]
                      .delegator_public_key.ToString() != "");
      TEST_ASSERT(result.era_summary.value()
                      .stored_value.era_info.value()
                      .seigniorage_allocations[i]
                      .amount >= 0);
    } else if (is_delegator == false && validator_cnt == 0) {
      validator_cnt++;
      TEST_ASSERT(result.era_summary.value()
                      .stored_value.era_info.value()
                      .seigniorage_allocations[i]
                      .validator_public_key.ToString() != "");
      TEST_ASSERT(result.era_summary.value()
                      .stored_value.era_info.value()
                      .seigniorage_allocations[i]
                      .amount >= 0);
    }
  }
}

/**
 * @brief Check the "state_get_item" rpc function
 *
 */
void get_item_test() {
  Casper::Client client(CASPER_TEST_ADDRESS);
  std::string state_root_hash =
      "39f2800688b94f68ca640b26c7d0f50a90d2ce9af55c9484e66151b544345303";
  std::string key =
      "transfer-"
      "9f5fe878c29fc3bf537c0509ec5abe1781a72bb6a3197a440e3e68247fba5909";

  // std::vector<std::string> path can be empty

  Casper::GetItemResult result = client.GetItem(state_root_hash, key);

  TEST_ASSERT(result.api_version != "");

  TEST_ASSERT(result.stored_value.transfer.has_value());
  TEST_ASSERT(result.stored_value.transfer.value().deploy_hash != "");
  TEST_ASSERT(result.stored_value.transfer.value().from.ToString() != "");
  TEST_ASSERT(result.stored_value.transfer.value().source.ToString() != "");
  TEST_ASSERT(result.stored_value.transfer.value().target.ToString() != "");
  TEST_ASSERT(result.stored_value.transfer.value().amount >= 0);
  TEST_ASSERT(result.stored_value.transfer.value().gas >= 0);

  TEST_ASSERT(result.merkle_proof != "");
}

// TODO: Write the other tests such as by NamedKey, Dictionary, etc.
/**
 * @brief Check the "state_get_dictionary_item" rpc function by URef
 *
 */
void get_dictionary_item_test() {
  Casper::Client client(CASPER_TEST_ADDRESS);
  std::string state_root_hash =
      "322b8d17faea2ee780b9b952a25a86520d36a78e20113f0658ae0b29a68a7384";

  std::string item_key =
      "dictionary-"
      "5d3e90f064798d54e5e53643c4fce0cbb1024aadcad1586cc4b7c1358a530373";

  Casper::GetDictionaryItemResult dictionaryItemResult =
      client.GetDictionaryItem(state_root_hash, item_key);

  TEST_ASSERT(dictionaryItemResult.api_version != "");

  TEST_ASSERT(dictionaryItemResult.dictionary_key != "");
  TEST_ASSERT(dictionaryItemResult.merkle_proof != "");
  TEST_ASSERT(dictionaryItemResult.stored_value.cl_value.has_value());
  TEST_ASSERT(dictionaryItemResult.stored_value.cl_value.value().bytes.size() >
              0);
  TEST_ASSERT(dictionaryItemResult.stored_value.cl_value.value()
                  .cl_type.type_info.has_value());
  uint8_t cl_type_val =
      static_cast<uint8_t>(dictionaryItemResult.stored_value.cl_value.value()
                               .cl_type.type_info.value()
                               .type);
  TEST_ASSERT(cl_type_val == 10);
}

/**
 * @brief Check the "state_get_balance" rpc function
 *
 */
void get_balance_test() {
  Casper::Client client(CASPER_TEST_ADDRESS);
  std::string purse_uref =
      "uref-54fd72455872082a254b0160e94a86245acd0c441f526688bda1261d0969057a-"
      "007";
  std::string state_root_hash =
      "66eb7e43886c908aae8246ba2d22aa30d21e1c187a38fa3093f14e4a4219dd6c";

  Casper::GetBalanceResult result =
      client.GetAccountBalance(purse_uref, state_root_hash);

  TEST_ASSERT(result.api_version != "");
  TEST_ASSERT(result.balance_value >= 0);
  TEST_ASSERT(result.merkle_proof != "");
}

/**
 * @brief Check the "state_get_auction_info" rpc function
 *
 */
void get_auction_info_test() {
  Casper::Client client(CASPER_TEST_ADDRESS);

  std::string block_hash =
      "a5ce9e1ea4ff786cf1eb9dfbe3a79f70ae33d723134a060910a2db80daf85bab";

  Casper::GetAuctionInfoResult auction_result =
      client.GetAuctionInfo(block_hash);

  TEST_ASSERT(auction_result.api_version != "");
  TEST_ASSERT(auction_result.auction_state.state_root_hash != "");
  TEST_ASSERT(auction_result.auction_state.block_height > 0);

  TEST_ASSERT(auction_result.auction_state.era_validators.size() > 0);
  TEST_ASSERT(auction_result.auction_state.era_validators[0].era_id > 0);
  TEST_ASSERT(
      auction_result.auction_state.era_validators[0].validator_weights.size() >
      0);
  TEST_ASSERT(auction_result.auction_state.era_validators[0]
                  .validator_weights[0]
                  .public_key.ToString() != "");
  TEST_ASSERT(auction_result.auction_state.era_validators[0]
                  .validator_weights[0]
                  .weight > 0);

  TEST_ASSERT(auction_result.auction_state.bids.size() > 0);
  TEST_ASSERT(auction_result.auction_state.bids[0].public_key.ToString() != "");

  TEST_ASSERT(auction_result.auction_state.bids[0]
                  .bid.validator_public_key.ToString() != "");
  TEST_ASSERT(
      auction_result.auction_state.bids[0].bid.bonding_purse.ToString() != "");
  TEST_ASSERT(auction_result.auction_state.bids[0].bid.staked_amount > 0);
  TEST_ASSERT(auction_result.auction_state.bids[0].bid.delegation_rate > 0);
}

/// <summary>
/// Check the Casper lower-case convert function
/// </summary>
void string_util_toLower_test() {
  std::string str = "Hello World";
  std::string str_lower = "hello world";
  TEST_ASSERT(str_lower == Casper::StringUtil::toLower(str));
}

/// <summary>
/// Check the public key to account hash convert function
/// </summary>
void publicKeyGetAccountHashTest() {
  Casper::PublicKey publicKey = Casper::PublicKey::FromHexString(
      "01cd807fb41345d8dd5a61da7991e1468173acbee53920e4dfe0d28cb8825ac664");

  std::string lower_case_account_hash = publicKey.GetAccountHash();
  Casper::StringUtil::toLower(lower_case_account_hash);

  std::string expected_account_hash =
      "account-hash-"
      "998c5fd4e7b568bedd78e05555c83c61893dc5d8546ce0bec8b30e1c570f21aa";

  TEST_ASSERT(lower_case_account_hash == expected_account_hash);
}

// Optional TODO:
// 1. CryptoUtil functions tests
// 2. Other StringUtil functions tests
// 3. CEP57 Checksum tests

TEST_LIST = {
    {"peers", get_peers_test},
    {"root hash 1", get_state_root_hash_block_height_test},
    {"root hash 2", get_state_root_hash_test},
    {"infoGetDeploy", get_deploy_test},
    {"infoGetStatus", get_status_info_test},
    {"infoGetBlockTransfers", get_block_transfers_test},
    {"chainGetBlock", get_block_test},
    {"chainGetEraInfoBySwitchBlock", get_era_info_by_switch_block_test},
    {"stateGetItem", get_item_test},
    {"stateGetDictionaryItem", get_dictionary_item_test},
    {"stateGetBalance", get_balance_test},
    {"stateGetAuctionInfo", get_auction_info_test},
    {"StringUtil - ToLower", string_util_toLower_test},
    {"PublicKey - GetAccountHash", publicKeyGetAccountHashTest},
    {NULL, NULL}};
