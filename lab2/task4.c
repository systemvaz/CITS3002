unsigned short mychecksum(unsigned char *in, int length)
{
  register int myshort = 0;
  register int i;
  for (i = 0; i < length; i++)
  {
    myshort = myshort + in[i];
  }
  return myshort;
}
