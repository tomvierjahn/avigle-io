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

#ifndef AVIGLE__IO__RMV_WRITER_H_
#define AVIGLE__IO__RMV_WRITER_H_


#include <fstream>
#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <io/io_api.h>
#include <io/output_adapter_interface.h>


namespace io
{

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
class IO_API RmvWriter
{
  friend class OutputData;

private:
  enum RmvVersion
  {
    kRmvVersion010 = 1
  };

  RmvWriter(const std::string& fileName, RmvVersion rmvVersion = kRmvVersion010);
  ~RmvWriter();

  template <typename FloatType>
  void Write(OutputAdapterInterface<FloatType>* pOutputAdapter);

  template <typename FloatType>
  void WriteVersion1(OutputAdapterInterface<FloatType>* pOutputAdapter);

  boost::filesystem::path fOutputPath;
  RmvVersion fVersion;
};  // class





////////////////////////////////////////////////////////////////////////////////
/// Loader
////////////////////////////////////////////////////////////////////////////////
template <typename FloatType>
void
RmvWriter::Write
(OutputAdapterInterface<FloatType>* pOutputAdapter)
{
	if (this->fVersion == kRmvVersion010)
	{
		this->WriteVersion1(pOutputAdapter);
	}
}



template <typename FloatType>
void
RmvWriter::WriteVersion1
(OutputAdapterInterface<FloatType>* pOutputAdapter)
{
  std::ofstream ofs;
  ofs.open(this->fOutputPath.c_str());

  const std::string delimiter = ";";

  // write version of RMV file
  ofs << "RMV_1" << std::endl;

  // additional empty line (see file format, wiki)
  ofs << std::endl;

  // write the number of textures
  unsigned int numTex = pOutputAdapter->CountTextures();
  ofs << numTex << std::endl;

  // write the textures
  for (unsigned int i=0; i<numTex; ++i)
  {
    pOutputAdapter->FetchNextTexture();

    unsigned int texId;
    pOutputAdapter->GetTextureID(&texId);

    std::string filename;
    pOutputAdapter->GetTextureFilename(&filename);

    unsigned int width, height;
    pOutputAdapter->GetTextureSize(&width, &height);

    FloatType posX, posY, posZ;
    pOutputAdapter->GetTexturePosition(&posX, &posY, &posZ);

    FloatType dirX, dirY, dirZ;
    pOutputAdapter->GetTextureDirection(&dirX, &dirY, &dirZ);

    ofs << texId
        << delimiter << filename
        << delimiter << width
        << delimiter << height
        << delimiter << posX
        << delimiter << posY
        << delimiter << posZ
        << delimiter << dirX
        << delimiter << dirY
        << delimiter << dirZ
        << std::endl;
  }

  // additional empty line (see file format, wiki)
  ofs << std::endl;


  // write number of points
  unsigned int numPts = pOutputAdapter->CountPoints();
  ofs << numPts << std::endl;

  // write points
  for (int i=0; i<numPts; ++i)
  {
    pOutputAdapter->FetchNextPoint();

    FloatType x,y,z;
    pOutputAdapter->GetPointPosition(&x, &y, &z);

    FloatType r,g,b;
    pOutputAdapter->GetPointColour(&r, &g, &b);

    FloatType conf;
    pOutputAdapter->GetPointConfidence(&conf);

    ofs << x
        << delimiter << y
        << delimiter << z
        << delimiter << r
        << delimiter << g
        << delimiter << b
        << delimiter << conf;

    // retrieve number of tex coordinates for this 3D point
    unsigned int numPtTexCoords = pOutputAdapter->CountPointTextureCoordinates();

    ofs << delimiter << numPtTexCoords;

    // retrieve tex coordinates for this 3D point and write to stream
    unsigned int ptTexId;
    FloatType u,v;
    for (int j=0; j<numPtTexCoords; ++j)
    {
      pOutputAdapter->FetchNextPointTextureCoordinate();

      pOutputAdapter->GetPointTextureCoordinate(&ptTexId, &u, &v);

      ofs << delimiter << ptTexId
          << delimiter << u
          << delimiter << v;
    }

    ofs << std::endl;
  }

  if (ofs.is_open())
  {
    ofs.close();
  }
}


} // namespace io


#endif  // #ifdef AVIGLE__IO__RMV_WRITER_H_
