/*
 * Copyright (C) 2017-2018 Dremio Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <map>
#include <typeinfo>
#include "codegen/dex.h"

namespace gandiva {

class TestDex : public ::testing::Test {
 protected:
  void SetUp() {
    name_map_[&typeid(VectorReadValidityDex)] = "VectorReadValidityDex";
    name_map_[&typeid(VectorReadValueDex)] = "VectorReadValueDex";
    name_map_[&typeid(NonNullableFuncDex)] = "NonNullableFuncDex";
    name_map_[&typeid(NullableNeverFuncDex)] = "NullableNeverFuncDex";
    name_map_[&typeid(LiteralDex)] = "LiteralDex";
  }

  std::map<const std::type_info *, std::string> name_map_;
};

TEST_F(TestDex, TestVisitor) {
  class TestVisitor : public DexVisitor {
   public:
    TestVisitor(std::map<const std::type_info *, std::string> *map,
                std::string *result)
      : map_(map),
        result_(result) {}

    void Visit(const VectorReadValidityDex &dex) override {
      *result_ = (*map_)[&typeid(dex)];
    }

    void Visit(const VectorReadValueDex &dex) override {
      *result_ = (*map_)[&typeid(dex)];
    }

    void Visit(const LiteralDex &dex) override {
      *result_ = (*map_)[&typeid(dex)];
    }

    void Visit(const NonNullableFuncDex &dex) override {
      *result_ = (*map_)[&typeid(dex)];
    }

    void Visit(const NullableNeverFuncDex &dex) override {
      *result_ = (*map_)[&typeid(dex)];
    }

   private:
    std::map<const std::type_info *, std::string> *map_;
    std::string *result_;
  };

  std::string desc;
  TestVisitor visitor(&name_map_, &desc);

  FieldPtr field = arrow::field("abc", arrow::int32());
  FieldDescriptorPtr field_desc = std::make_shared<FieldDescriptor>(field, 0, 1, 2);
  VectorReadValidityDex vv_dex(field_desc);
  vv_dex.Accept(&visitor);
  EXPECT_EQ(desc, name_map_[&typeid(VectorReadValidityDex)]);

  VectorReadValueDex vd_dex(field_desc);
  vd_dex.Accept(&visitor);
  EXPECT_EQ(desc, name_map_[&typeid(VectorReadValueDex)]);

  std::vector<DataTypePtr> params{arrow::int32()};
  FuncDescriptorPtr my_func =
      std::make_shared<FuncDescriptor>("abc", params, arrow::boolean());

  NonNullableFuncDex non_nullable_func(my_func, NULL, {NULL});
  non_nullable_func.Accept(&visitor);
  EXPECT_EQ(desc, name_map_[&typeid(NonNullableFuncDex)]);

  NullableNeverFuncDex nullable_func(my_func, NULL, {NULL});
  nullable_func.Accept(&visitor);
  EXPECT_EQ(desc, name_map_[&typeid(NullableNeverFuncDex)]);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

} // namespace gandiva