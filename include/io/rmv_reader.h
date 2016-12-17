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

#ifndef AVIGLE__IO__RMV_READER_H_
#define AVIGLE__IO__RMV_READER_H_


#include <fstream>
#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <io/io_api.h>
#include <io/input_adapter_interface.h>
#include <io/reader_tools.h>


namespace io
{

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
class IO_API RmvReader
{
  friend class InputData;

private:
  enum RmvVersion
  {
    kRmvVersion010 = 1,
    kRmvVersionInvalid
  };

  RmvReader(const std::string& fileName);
  ~RmvReader();

  template <typename FloatType>
  void Load(InputAdapterInterface<FloatType>* pInputAdapter);

  boost::filesystem::path fInputPath;
  RmvVersion fVersion;
};  // class





////////////////////////////////////////////////////////////////////////////////
/// Loader
////////////////////////////////////////////////////////////////////////////////
template <typename FloatType>
void
RmvReader::Load
(InputAdapterInterface<FloatType>* pInputAdapter)
{
  namespace iort = io::ReaderTools;

  // open
  std::ifstream inputStream(this->fInputPath.c_str());
  if (!inputStream.is_open())
  {
    std::cerr << "Could not open input file " << this->fInputPath << "!";
    std::cerr << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }

  // determine version
  const std::string version(iort::Line<std::string>(inputStream));
  if(version.compare("RMV_1") == 0)
  {
    this->fVersion = io::RmvReader::kRmvVersion010;
  }
  else
  {
    std::cerr << "Input file " << this->fInputPath << " not a valid RMV file!";
    std::cerr << std::endl;
    std::cerr << "Terminating." << std::endl;
    exit(EXIT_FAILURE);
  }

  // TEXTURES
  const unsigned int numOfTextures = iort::Line<unsigned int>(inputStream);
  for (unsigned int texNum = 0; texNum < numOfTextures; ++texNum)
  {
    iort::Tokens textureTokens(iort::Line<std::string>(inputStream), ";");
    const unsigned int texId = iort::Token<unsigned int>(textureTokens);
    const std::string texFileName(iort::Token<std::string>(textureTokens));
    const unsigned int texWidth = iort::Token<unsigned int>(textureTokens);
    const unsigned int texHeight = iort::Token<unsigned int>(textureTokens);
    const FloatType camPosX = iort::Token<FloatType>(textureTokens);
    const FloatType camPosY = iort::Token<FloatType>(textureTokens);
    const FloatType camPosZ = iort::Token<FloatType>(textureTokens);
    const FloatType camDirX = iort::Token<FloatType>(textureTokens);
    const FloatType camDirY = iort::Token<FloatType>(textureTokens);
    const FloatType camDirZ = iort::Token<FloatType>(textureTokens);

    pInputAdapter->OnTexture(
      texId,   // id
      boost::filesystem::absolute(                // file name
        boost::filesystem::path(
          texFileName),
          this->fInputPath.parent_path()).string(),
      texWidth, texHeight,
      camPosX, camPosY, camPosZ,
      camDirX, camDirY, camDirZ,
      static_cast<FloatType>(1.0),
      static_cast<FloatType>(0.0),
      static_cast<FloatType>(0.0),
      static_cast<FloatType>(0.0),
      static_cast<FloatType>(1.0),
      static_cast<FloatType>(0.0),
      static_cast<FloatType>(0.0),
      static_cast<FloatType>(0.0),
      static_cast<FloatType>(1.0),
      static_cast<FloatType>(0.0),
      static_cast<FloatType>(0.0),
      static_cast<FloatType>(0.0),
      static_cast<FloatType>(0.0),
      static_cast<FloatType>(0.0)
    );
  }

  // POINTS
  const unsigned int numOfPoints = iort::Line<unsigned int>(inputStream);
  for (unsigned int pointNum = 0; pointNum < numOfPoints; ++pointNum)
  {
    iort::Tokens pointTokens(iort::Line<std::string>(inputStream), ";");
    const FloatType positionX = iort::Token<FloatType>(pointTokens);
    const FloatType positionY = iort::Token<FloatType>(pointTokens);
    const FloatType positionZ = iort::Token<FloatType>(pointTokens);
    const FloatType colourR = iort::Token<FloatType>(pointTokens);
    const FloatType colourG = iort::Token<FloatType>(pointTokens);
    const FloatType colourB = iort::Token<FloatType>(pointTokens);

    pInputAdapter->OnBeginPoint();
    pInputAdapter->OnPointPosition(positionX, positionY, positionZ);
    pInputAdapter->OnPointColour(colourR, colourG, colourB);

    iort::Token<iort::Unused>(pointTokens); // confidence --> not yet used

    // tex coords per point
    const unsigned int numCoords = iort::Token<unsigned int>(pointTokens);
    for (unsigned int texCoord = 0; texCoord < numCoords; ++texCoord)
    {
      const unsigned int texCoordId = iort::Token<unsigned int>(pointTokens);
      const FloatType texCoordU = iort::Token<FloatType>(pointTokens);
      const FloatType texCoordV = iort::Token<FloatType>(pointTokens);

      pInputAdapter->OnPointTexCoord(texCoordId, texCoordU, texCoordV);
    }
    pInputAdapter->OnEndPoint();
  }

  if (inputStream.is_open())
  {
    inputStream.close();
  }
}


} // namespace io


#endif  // #ifdef AVIGLE__IO__RMV_READER_H_
