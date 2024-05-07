/*
 * file: utBoostFile.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "utility/BoostFile.h"
#include "Logging.h"

class utBoostFile : public testing::Test
{
public:
	virtual void SetUp()
	{
		BoostFile("TEST.bin").Delete();
		BoostFile("TEST1.bin").Delete();
	}
};

TEST_F(utBoostFile,CheckUnmapfile)
{
	BoostFile	bf("TEST.bin");
	EXPECT_FALSE(bf.IsExists());
	EXPECT_FALSE(bf.IsRegularFile());
	EXPECT_FALSE(bf.Delete());
	EXPECT_FALSE(0 < bf.GetFileSize());
	EXPECT_FALSE(bf.RenameAs("TEST1.bin"));
	EXPECT_FALSE(bf.CopyTo("TEST1.bin"));
}

TEST_F(utBoostFile,CheckMapfile)
{
	BoostFile	bf("TEST.bin");
	EXPECT_TRUE(bf.CreateAndFillFile(4096,0));
	EXPECT_TRUE(bf.IsExists());
	EXPECT_TRUE(bf.IsRegularFile());
	EXPECT_EQ(bf.GetFileSize(),4096);
	EXPECT_TRUE(bf.RenameAs("TEST1.bin"));
	EXPECT_TRUE(bf.CopyTo("TEST.bin"));
	EXPECT_TRUE(bf.Delete());
}
