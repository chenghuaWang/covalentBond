#include "cbOp.hpp"

using namespace rapidjson;

void __parseKeyTypeFromJson(cbOperator* ele, const std::string keyType) {
  Document d;
  d.Parse(keyType.c_str());
  // Get the inputs values' nums
  int32_t inputsNums = d["inputsValueNums"].GetInt();
  // Get the sentence part.
  Value& __sentence = d["sentence"];
  for (int32_t i = 0; i < inputsNums; ++i) {
    ele->m_sentence.push_back(
        __sentence[std::string("atom_" + std::to_string(i)).c_str()].GetString());
  }
  // Get key and value of Inputs.
  Value& __inputs = d["inputs"];
  for (int32_t i = 0; i < inputsNums; ++i) {
    std::unordered_map<std::string, std::string> _tmp;
    auto& __atom = __inputs[std::string("atom_" + std::to_string(i)).c_str()];
    for (Value::ConstMemberIterator itr = __atom.MemberBegin(); itr != __atom.MemberEnd(); ++itr) {
      _tmp[itr->name.GetString()] = itr->value.GetString();
    };
    ele->m_keyTypeIn.push_back(_tmp);
  }
  // Get Key and value of Outputs.
  Value& __outputs = d["outputs"];
  for (Value::ConstMemberIterator itr = __outputs.MemberBegin(); itr != __outputs.MemberEnd();
       ++itr) {
    ele->m_KeyTypeOut[itr->name.GetString()] = itr->value.GetString();
  }
}

cbOperator::cbOperator(const std::string keyType, const std::string luaOp)
    : m_luaJitHandle(MAKE_SHARED(luaJitThread)()) {
  __parseKeyTypeFromJson(this, keyType);
  m_luaJitHandle->loadScript2Mem(luaOp);
}