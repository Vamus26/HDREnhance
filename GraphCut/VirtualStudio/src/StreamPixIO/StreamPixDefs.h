#ifndef __STREAM_PIX_H__
#define __STREAM_PIX_H__

#include "Definitions.h"

const string STREAM_PIX_SeqExt     = ".seq";
const string STREAM_PIX_SeqLinkExt = ".lnk";

#ifndef BYTE
typedef unsigned char BYTE;
#endif

enum eIMAGEFORMAT
{
    FORMAT_UNKNOWN=0,
    FORMAT_MONO=100,
    FORMAT_MONO_BAYER=101,
    FORMAT_BGR_PACKED=200,
    FORMAT_BGR_PLANAR=300,		// Actually RGB plannar
    FORMAT_RGB_PACKED=400,
    FORMAT_BGRx_PACKED=500,
    FORMAT_YUV422_PACKED=600,
    FORMAT_UVY422_PACKED=700,
    FORMAT_UVY411_PACKED=800,
    FORMAT_UVY444_PACKED=900,
    FORMAT_BASLER_VENDOR_SPECIFIC=1000
};

struct CImageInfo
{
    unsigned long ImageWidth;       //@- Image width in pixel
    unsigned long ImageHeight;      //@- Image height in pixel
    unsigned long ImageBitDepth;    //@- Image depth in bits (8,16,24,32)
    unsigned long ImageBitDepthReal;//@- Precise Image depth (x bits)
    unsigned long ImageSizeBytes;   //@- Size used to store one image.
    eIMAGEFORMAT ImageFormat;       //@- Planar, Packed, color or mono
    
    CImageInfo()
    {
        DefaultIni();
    }
    
    CImageInfo(unsigned long w, unsigned long h, unsigned long bd, unsigned long bdr, 
        unsigned long size, eIMAGEFORMAT f)
    {
        ImageWidth = w;
        ImageHeight = h;
        ImageBitDepth = bd;
        ImageBitDepthReal = bdr;
        ImageSizeBytes = size;
        ImageFormat = f;
    }

    /**Initialise with default values*/
    void DefaultIni()
    {
        ImageWidth = 0;
        ImageHeight = 0;
        ImageBitDepth = 0;
        ImageBitDepthReal = 0;
        ImageSizeBytes = 0;
        ImageFormat = FORMAT_UNKNOWN;
    }

    bool operator==(const CImageInfo& info)
    {
        return ImageWidth == info.ImageWidth
            && ImageHeight ==  info.ImageHeight
            && ImageBitDepth == info.ImageBitDepth
            && ImageBitDepthReal == info.ImageBitDepthReal
            && ImageSizeBytes == info.ImageSizeBytes
            && ImageFormat == info.ImageFormat;
    }

    bool operator!=(const CImageInfo& info)
    {
        return ImageWidth != info.ImageWidth
            || ImageHeight !=  info.ImageHeight
            || ImageBitDepth != info.ImageBitDepth
            || ImageBitDepthReal != info.ImageBitDepthReal
            || ImageSizeBytes != info.ImageSizeBytes
            || ImageFormat != info.ImageFormat;
    }
};


struct CSequenceHeader
{                                                       
    //Those members are always present in every version and must stay in this order
    long MagicNumber;                   //@- Cool magic number (always 0xFEED)

    //@- Always "Norpix Seq\n"
    char Name[24]; //24 * 1 byte/char = 24 bytes

    long Version; //@- Sequence Header Version
    long HeaderSize; //@- The size of the header

    //@- User description
    char Description[512]; //512 * 1 byte/char = 512 bytes
    
    //Those members can exist or not depending of the header version
    CImageInfo ImageInfo;               //@- Image information
    unsigned long AllocatedFrame;       //@- Number of frames allocated
    unsigned long Origin;               //@- First frame to use for playback
    unsigned long TrueImageSize;        //@- Number of bytes between the first pixel and first of next image
    double FrameRate;                   //@- Suggested Frame rate for playback
    BYTE Padding[432];                  //@- Padding
    
    CSequenceHeader()
    {
        DefaultIni();
    }
    
    /**Initialise with default values*/
    void DefaultIni()
    {
        MagicNumber = 0xFEED;                       //4 bytes
        sprintf(Name, "%s","Norpix seq\n");         //24 bytes
        Version = 3;                                //4 bytes
        HeaderSize = sizeof(CSequenceHeader);       //4 bytes
		sprintf(Description, "%s", "No Description\n"); //512 bytes
        ImageInfo.DefaultIni();                     //6 members * 4 bytes = 24 bytes
        AllocatedFrame = 0;                         //4 bytes
        Origin = 0;                                 //4 bytes
        FrameRate = 30.0;                           //8 bytes (Watch out for the alignment)
        TrueImageSize = 0;                          //4 bytes
                                                    //432 padding bytes = 1024 bytes
	}
};

struct CImageData
{
    void* RedChannel;   //@- Red planar or RGB packed
    void* GreenChannel; //@- Green planar or NULL packed
    void* BlueChannel;  //@- Blue planat or NULL packed
    
    time_t TimeStamp;     //the time stamp in time_t format
    unsigned short TimeStampMS; //timestamp milliseconds
    
    CImageData() 
    {
        RedChannel = 0; 
        GreenChannel = 0; 
        BlueChannel = 0; 
        TimeStamp = 0; 
        TimeStampMS = 0;
    }

    bool operator==(const CImageData& data)
    {
        return RedChannel == data.RedChannel
            && GreenChannel == data.GreenChannel
            && BlueChannel == data.BlueChannel
            && TimeStamp == data.TimeStamp
            && TimeStampMS == data.TimeStampMS;
    }

    bool operator!=(const CImageData& data)
    {
        return RedChannel != data.RedChannel
            || GreenChannel != data.GreenChannel
            || BlueChannel != data.BlueChannel
            || TimeStamp != data.TimeStamp
            || TimeStampMS != data.TimeStampMS;
    }
};

 



#endif