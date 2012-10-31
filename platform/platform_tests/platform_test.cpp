#include "../../testing/testing.hpp"

#include "../platform.hpp"

#include "../../defines.hpp"

#include "../../coding/file_writer.hpp"
#include "../../coding/internal/file_data.hpp"

#include "../../base/logging.hpp"


char const * TEST_FILE_NAME = "some_temporary_unit_test_file.tmp";

UNIT_TEST(WritableDir)
{
  string const path = GetPlatform().WritableDir() + TEST_FILE_NAME;

  try
  {
    my::FileData f(path, my::FileData::OP_WRITE_TRUNCATE);
  }
  catch (Writer::OpenException const &)
  {
    LOG(LCRITICAL, ("Can't create file"));
    return;
  }

  my::DeleteFileX(path);
}

UNIT_TEST(WritablePathForFile)
{
  Platform & pl = GetPlatform();
  string const p1 = pl.WritableDir() + TEST_FILE_NAME;
  string const p2 = pl.WritablePathForFile(TEST_FILE_NAME);
  TEST_EQUAL(p1, p2, ());
}

UNIT_TEST(GetReader)
{
  char const * NON_EXISTING_FILE = "mgbwuerhsnmbui45efhdbn34.tmp";
  char const * arr[] = { "drules_proto.txt", "basic_ldpi.skn", "classificator.txt", "minsk-pass.mwm" };
  Platform & p = GetPlatform();
  for (size_t i = 0; i < ARRAY_SIZE(arr); ++i)
  {
    ReaderPtr<Reader> r = p.GetReader(arr[i]);
    TEST_GREATER(r.Size(), 0, ("File should exist!"));
  }

  bool wasException = false;
  try
  {
    ReaderPtr<Reader> r = p.GetReader(NON_EXISTING_FILE);
  }
  catch (FileAbsentException const &)
  {
    wasException = true;
  }
  TEST_EQUAL(wasException, true, ());
}

UNIT_TEST(GetFilesInDir_Smoke)
{
  Platform & pl = GetPlatform();
  Platform::FilesList files1, files2;

  string const dir = pl.WritableDir();

  pl.GetFilesByExt(dir, DATA_FILE_EXTENSION, files1);
  TEST_GREATER(files1.size(), 0, ("/data/ folder should contain some data files"));

  pl.GetFilesByRegExp(dir, ".*\\" DATA_FILE_EXTENSION "$", files2);
  TEST_EQUAL(files1, files2, ());

  files1.clear();
  pl.GetFilesByExt(dir, ".dsa", files1);
  TEST_EQUAL(files1.size(), 0, ());
}

UNIT_TEST(GetFileSize)
{
  Platform & pl = GetPlatform();
  uint64_t size = 123141;
  TEST(!pl.GetFileSizeByName("adsmngfuwrbfyfwe", size), ());
  TEST(!Platform::GetFileSizeByFullPath("adsmngfuwrbfyfwe", size), ());

  {
    FileWriter testFile(TEST_FILE_NAME);
    testFile.Write("HOHOHO", 6);
  }
  size = 0;
  TEST(Platform::GetFileSizeByFullPath(TEST_FILE_NAME, size), ());
  TEST_EQUAL(size, 6, ());

  FileWriter::DeleteFileX(TEST_FILE_NAME);

  {
    FileWriter testFile(pl.WritablePathForFile(TEST_FILE_NAME));
    testFile.Write("HOHOHO", 6);
  }
  size = 0;
  TEST(pl.GetFileSizeByName(TEST_FILE_NAME, size), ());
  TEST_EQUAL(size, 6, ());

  FileWriter::DeleteFileX(pl.WritablePathForFile(TEST_FILE_NAME));
}

UNIT_TEST(CpuCores)
{
  int const coresNum = GetPlatform().CpuCores();
  TEST_GREATER(coresNum, 0, ());
  TEST_LESS_OR_EQUAL(coresNum, 128, ());
}

UNIT_TEST(GetWritableStorageStatus)
{
  TEST_EQUAL(Platform::STORAGE_OK, GetPlatform().GetWritableStorageStatus(100000), ());
  TEST_EQUAL(Platform::NOT_ENOUGH_SPACE, GetPlatform().GetWritableStorageStatus(uint64_t(-1)), ());
}
