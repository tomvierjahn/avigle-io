//------------------------------------------------------------------------------
// avigle-io -- common io classes/tools
//
// Developed during the research project AVIGLE
// which was part of the Hightech.NRW research program
// funded by the ministry for Innovation, Science, Research and Technology
// of the German state Northrhine-Westfalia, and by the European Union.
//
// Copyright (c) 2010--2013, Tom Vierjahn et al.
//------------------------------------------------------------------------------
//                                License
//
// This library/program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// If you are using this library/program in a project, work or publication,
// please cite [1,2].
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------------
//                                References
//
// [1] S. Rohde, N. Goddemeier, C. Wietfeld, F. Steinicke, K. Hinrichs,
//     T. Ostermann, J. Holsten, D. Moormann:
//     "AVIGLE: A System of Systems Concept for an
//      Avionic Digital Service Platform based on
//      Micro Unmanned Aerial Vehicles".
//     In Proc. IEEE Int'l Conf. Systems Man and Cybernetics (SMC),
//     pp. 459--466. 2010. DOI: 10.1109/ICSMC.2010.5641767
// [2] S. Strothoff, D. Feldmann, F. Steinicke, T. Vierjahn, S. Mostafawy:
//     "Interactive generation of virtual environments using MUAVs".
//     In Proc. IEEE Int. Symp. VR Innovations, pp. 89--96, 2011.
//     DOI: 10.1109/ISVRI.2011.5759608
//------------------------------------------------------------------------------

#include <cstdlib>

#include <boost/filesystem.hpp>


#include <io/nvm_reader.h>


namespace io
{

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
NvmReader::NvmReader
(const std::string& fileName)
: fInputPath(fileName)
, fVersion(io::NvmReader::kNvmVersionInvalid)
{
  namespace bf = boost::filesystem;

  // check if exists
  if (!bf::exists(this->fInputPath))
  {
    std::cerr << "Input file " << this->fInputPath << " does not exists!" << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
NvmReader::~NvmReader
()
{
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
void
NvmReader::GetJpegSize
(const std::string& fileName,
 unsigned int* width,
 unsigned int* height)
{
  namespace bf = boost::filesystem;
  bf::path imagePath(fileName);
  if (!bf::exists(imagePath))
  {
    std::cerr << "Texture file " << imagePath << " not found!" << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }

  // open
  std::ifstream jpegFile(imagePath.c_str());
  if (!jpegFile.is_open())
  {
    std::cerr << "Could not open texture file " << imagePath << "!" << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }

  bool error = false;

  char pBuffer[9];
  unsigned int jpegFilePos = jpegFile.tellg();

  // check first 4 bytes
  jpegFile.read(pBuffer, 5);
  if ((pBuffer[0] == static_cast<char>(0xFF)) &&
      (pBuffer[1] == static_cast<char>(0xD8)) &&
      (pBuffer[2] == static_cast<char>(0xFF)) &&
      (pBuffer[3] == static_cast<char>(0xE0)))
  {
    jpegFilePos += 4;
    jpegFile.seekg(jpegFilePos);
    jpegFile.read(pBuffer, 7);

    // check JFIF\0
    if ((pBuffer[2] == 'J') && (pBuffer[3] == 'F') &&
        (pBuffer[4] == 'I') && (pBuffer[5] == 'F') &&
        (pBuffer[6] == 0x00))
    {
      unsigned int blockLength =
        static_cast<unsigned int>(static_cast<unsigned char>(pBuffer[0])) *
          256 +
        static_cast<unsigned int>(static_cast<unsigned char>(pBuffer[1]));

      while (jpegFile.good())
      {
        jpegFilePos += blockLength;
        jpegFile.seekg(jpegFilePos);
        jpegFile.read(pBuffer, 9);

        // no beginning of a block
        if (pBuffer[0] != static_cast<char>(0xFF))
        {
          error = true;
          break;
        }

        // this is the FFC0 block containing the size
        if (pBuffer[1] == static_cast<char>(0xC0))
        {
          *height =
            static_cast<unsigned int>(static_cast<unsigned char>(pBuffer[5])) *
              256 +
            static_cast<unsigned int>(static_cast<unsigned char>(pBuffer[6]));
          *width =
            static_cast<unsigned int>(static_cast<unsigned char>(pBuffer[7])) *
              256 +
            static_cast<unsigned int>(static_cast<unsigned char>(pBuffer[8]));
          break;
        }
        // proceed to the next block
        else
        {
          jpegFilePos += 2;
          jpegFile.seekg(jpegFilePos);
          jpegFile.read(pBuffer, 2);

          blockLength =
            static_cast<unsigned int>(static_cast<unsigned char>(pBuffer[0])) *
              256 +
            static_cast<unsigned int>(static_cast<unsigned char>(pBuffer[1]));
        }
      }
    }
    else
    {
      error = true;
    }
  }
  else
  {
    error = true;
  }

  if (error)
  {
    std::cerr << "Error reading file " << imagePath << "!" << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }
  if (jpegFile.is_open())
  {
    jpegFile.close();
  }
}


} // namespace io
