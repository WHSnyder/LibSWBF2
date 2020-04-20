// LibSWBF2.Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include "CImg.h"

using MSH = LibSWBF2::Chunks::Mesh::MSH;
using TERR = LibSWBF2::Chunks::Terrain::TERR;

void DumpMSH()
{
	MSH* msh = MSH::Create();
	const size_t size = sizeof(MSH);

	std::cout << "sizeof(MSH) : " << size << std::endl;

	std::ofstream stream;
	stream.open("MSHdump.bin", std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
	const bool success = stream.good() && stream.is_open();
	if (!success)
	{
		std::cout << "could not create file!" << std::endl;
		stream.close();

		MSH::Destroy(msh);
		return;
	}
	stream.write((char*)msh, size);
	stream.close();
	std::cout << "dumped binary!" << std::endl;

	MSH::Destroy(msh);
}

void TerrToImg()
{
//	TERR* terrain = TERR::Create();
//
//#pragma pack(push, 1)
//	struct BMPFileHeader {
//		uint16_t file_type{ 0x4D42 };          // File type always BM which is 0x4D42
//		uint32_t file_size{ 0 };               // Size of the file (in bytes)
//		uint16_t reserved1{ 0 };               // Reserved, always 0
//		uint16_t reserved2{ 0 };               // Reserved, always 0
//		uint32_t offset_data{ 0 };             // Start position of pixel data (bytes from the beginning of the file)
//	};
//
//	struct BMPInfoHeader {
//		uint32_t size{ 0 };                     // Size of this header (in bytes)
//		int32_t width{ 0 };                     // width of bitmap in pixels
//		int32_t height{ 0 };                    // height of bitmap in pixels
//												//       (if positive, bottom-up, with origin in lower left corner)
//												//       (if negative, top-down, with origin in upper left corner)
//		uint16_t planes{ 1 };                   // No. of planes for the target device, this is always 1
//		uint16_t bit_count{ 0 };                // No. of bits per pixel
//		uint32_t compression{ 0 };              // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
//		uint32_t size_image{ 0 };               // 0 - for uncompressed images
//		int32_t x_pixels_per_meter{ 0 };
//		int32_t y_pixels_per_meter{ 0 };
//		uint32_t colors_used{ 0 };              // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
//		uint32_t colors_important{ 0 };         // No. of colors used for displaying the bitmap. If 0 all colors are required
//	};
//#pragma pack(pop)
//
//
//	TERR::Destroy(terrain);
}

int main()
{
	DumpMSH();
	//TerrToImg();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
