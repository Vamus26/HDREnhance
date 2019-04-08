#include "Definitions.h"

#ifndef __SEGMENT_MEM_H__
#define __SEGMENT_MEM_H__

inline void WRITE_BOOL(char **buffPtr, bool val)
{
	*((bool *) (*buffPtr)) = val;
	*buffPtr += sizeof(bool);
}

inline void WRITE_INT(char **buffPtr, int val)
{
	*((int *) (*buffPtr)) = val;
	*buffPtr += sizeof(int);
}

inline void WRITE_FLOAT(char **buffPtr, float val)
{
	*((float *) (*buffPtr)) = val;
	*buffPtr += sizeof(float);
}

inline void WRITE_SHORT(char **buffPtr, short val)
{
	*((short *) (*buffPtr)) = val;
	*buffPtr += sizeof(short);
}

inline void WRITE_USHORT(char **buffPtr, unsigned short val)
{
	*((unsigned short *) (*buffPtr)) = val;
	*buffPtr += sizeof(unsigned short);
}

inline void READ_BOOL(char **buffPtr, bool &val)
{
	val = *((bool *) (*buffPtr));
	*buffPtr += sizeof(bool);
}

inline void READ_INT(char **buffPtr, int &val)
{
	val = *((int *) (*buffPtr));
	*buffPtr += sizeof(int);
}

inline void READ_FLOAT(char **buffPtr, float &val)
{
	val = *((float *) (*buffPtr));
	*buffPtr += sizeof(float);
}

inline void READ_SHORT(char **buffPtr, short &val)
{
	val = *((short *) (*buffPtr));
	*buffPtr += sizeof(short);
}

inline void READ_USHORT(char **buffPtr, unsigned short &val)
{
	val = *((unsigned short *) (*buffPtr));
	*buffPtr += sizeof(unsigned short);
}

#endif
