int my_checksum(unsigned char* in, int length)
{
  int mychecksum = 0;
  for(int i = 0; i < length; i++)
  {
    mychecksum = mychecksum + in[i];
  }
  return mychecksum;
}
