#include "pch.h"
#include "../Core/WordGraph.h"
TEST(TestCaseName, TestName1) {
	char *words[] = { "dfsa", "adf","fg","glo" };
	char *result[4];
	//Core::gen_chain_word(words, vWords.size(), result, head, tail, wlOption.allowLoop);
	int len = Core::gen_chain_word(words, 4, result, 0,0,true);
	EXPECT_EQ(len, 4);
  EXPECT_TRUE(true);
}

TEST(TestCaseName, TestName2) {
	char *words[] = { "Algebra","apple","zoo","elephant","under","pseudopseudohypoparathyroidism","meach"
 };
	char *result[7];
	//Core::gen_chain_word(words, vWords.size(), result, head, tail, wlOption.allowLoop);
	int len = Core::gen_chain_char(words, 7, result, 0, 0, true);
	EXPECT_EQ(len, 2);
	EXPECT_TRUE(true);
}

TEST(TestCaseName, TestName3) {
	char *words[] = { "Algebra","Apple","zOo","elephant","under","pseudopseudohypoparathyroidism"
	};
	char *result[6];
	//Core::gen_chain_word(words, vWords.size(), result, head, tail, wlOption.allowLoop);
	int len = Core::gen_chain_word(words, 6, result, 0, 0, true);
	EXPECT_EQ(len, 3);
	EXPECT_TRUE(true);
}

TEST(TestCaseName, TestName4) {
	char *words[] = { "Algebra"
	};
	char *result[1];
	//Core::gen_chain_word(words, vWords.size(), result, head, tail, wlOption.allowLoop);
	int len = Core::gen_chain_word(words, 1, result, 0, 0, true);
	EXPECT_EQ(len, 0);
	EXPECT_TRUE(true);
}