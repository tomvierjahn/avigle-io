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

#include <fstream>
#include <iostream>

#include <tr1/functional>
#include <boost/function.hpp>
#include <boost/lambda/casts.hpp>
#include <boost/lambda/lambda.hpp>

#include <io/ply_reader.h>

#include <ply/ply.hpp>
#include <ply/ply_parser.hpp>


namespace io
{

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
PlyReader::PlyReader
(const std::string& fileName)
: fInputPath(fileName)
, fAdapterOnBeginPoint(NULL)
, fAdapterOnPointPosition(NULL)
, fAdapterOnPointNormal(NULL)
, fAdapterOnPointColour(NULL)
, fAdapterOnEndPoint(NULL)
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
PlyReader::~PlyReader
()
{
}





////////////////////////////////////////////////////////////////////////////////
/// Registers callbacks for the individual elements of the ply file
////////////////////////////////////////////////////////////////////////////////
io::PlyReader::ElementCallbackTuple
PlyReader::ElementDefinitionCallback
(const std::string& element_name, std::size_t)
{
  if (element_name == "vertex")
  {
    return io::PlyReader::ElementCallbackTuple(
      std::tr1::bind(&io::PlyReader::VertexBeginCallback, this),
      std::tr1::bind(&io::PlyReader::VertexEndCallback, this) );
  }
  else
  {
    return ElementCallbackTuple(NULL, NULL);
  }
}





////////////////////////////////////////////////////////////////////////////////
/// With this method the correct function pointers for handling the elements'
/// attributes are determined.
///
/// \note This method is similar to the one used in the examples of ply-loader.
////////////////////////////////////////////////////////////////////////////////
template <>
std::tr1::function<void (ply::float32)>
PlyReader::ScalarPropertyDefinitionCallback<ply::float32>
(const std::string& elementName,
 const std::string& propertyName)
{
  namespace bl = boost::lambda;

  if (elementName == "vertex")
  {
    if (propertyName == "x")
    {
      return boost::function<void (float)>(bl::var(this->fPositionX) = bl::ll_static_cast<double>(bl::_1));
    }
    else if (propertyName == "y")
    {
      return boost::function<void (float)>(bl::var(this->fPositionY) = bl::ll_static_cast<double>(bl::_1));
    }
    else if (propertyName == "z")
    {
      return boost::function<void (float)>(bl::var(this->fPositionZ) = bl::ll_static_cast<double>(bl::_1));
    }
    else if (propertyName == "nx")
    {
      return boost::function<void (float)>(bl::var(this->fNormalX) = bl::ll_static_cast<double>(bl::_1));
    }
    else if (propertyName == "ny")
    {
      return boost::function<void (float)>(bl::var(this->fNormalY) = bl::ll_static_cast<double>(bl::_1));
    }
    else if (propertyName == "nz")
    {
      return boost::function<void (float)>(bl::var(this->fNormalZ) = bl::ll_static_cast<double>(bl::_1));
    }
    else if (propertyName == "red" || propertyName == "diffuse_red")
    {
      return boost::function<void (float)>(bl::var(this->fColourR) = bl::ll_static_cast<double>(bl::_1));
    }
    else if (propertyName == "green" || propertyName == "diffuse_green")
    {
      return boost::function<void (float)>(bl::var(this->fColourG) = bl::ll_static_cast<double>(bl::_1));
    }
    else if (propertyName == "blue" || propertyName == "diffuse_blue")
    {
      return boost::function<void (float)>(bl::var(this->fColourB) = bl::ll_static_cast<double>(bl::_1));
    }
  }

  return 0;
}





////////////////////////////////////////////////////////////////////////////////
/// With this method the correct function pointers for handling the elements'
/// attributes are determined.
///
/// \note This method is similar to the one used in the examples of ply-loader.
////////////////////////////////////////////////////////////////////////////////
template <>
std::tr1::function<void (ply::uint8)>
PlyReader::ScalarPropertyDefinitionCallback<ply::uint8>
(const std::string& elementName,
 const std::string& propertyName)
{
  namespace bl = boost::lambda;

  if (elementName == "vertex")
  {
    if (propertyName == "red" || propertyName == "diffuse_red")
    {
      return boost::function<void (unsigned int)>(bl::var(this->fColourR) = bl::ll_static_cast<double>(bl::_1) / 255.0);
    }
    else if (propertyName == "green" || propertyName == "diffuse_green")
    {
      return boost::function<void (unsigned int)>(bl::var(this->fColourG) = bl::ll_static_cast<double>(bl::_1) / 255.0);
    }
    else if (propertyName == "blue" || propertyName == "diffuse_blue")
    {
      return boost::function<void (unsigned int)>(bl::var(this->fColourB) = bl::ll_static_cast<double>(bl::_1) / 255.0);
    }
  }

  return 0;
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
void
PlyReader::VertexBeginCallback
()
{
  this->fPositionX = 0.0;
  this->fPositionY = 0.0;
  this->fPositionZ = 0.0;
  this->fNormalX = 0.0;
  this->fNormalY = 0.0;
  this->fNormalZ = 0.0;
  this->fColourR = 1.0;
  this->fColourG = 1.0;
  this->fColourB = 1.0;
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
void
PlyReader::VertexEndCallback
()
{
  if (this->fAdapterOnBeginPoint != NULL &&
      this->fAdapterOnPointPosition != NULL &&
      this->fAdapterOnPointNormal != NULL &&
      this->fAdapterOnPointColour != NULL &&
      this->fAdapterOnEndPoint != NULL)
  {
    this->fAdapterOnBeginPoint();
    this->fAdapterOnPointPosition(this->fPositionX,
                                  this->fPositionY,
                                  this->fPositionZ);
    this->fAdapterOnPointNormal(this->fNormalX,
                                this->fNormalY,
                                this->fNormalZ);
    this->fAdapterOnPointColour(this->fColourR,
                                this->fColourG,
                                this->fColourB);
    this->fAdapterOnEndPoint();
  }
}





////////////////////////////////////////////////////////////////////////////////
/// Provides a callback for messages from the ply parsser
////////////////////////////////////////////////////////////////////////////////
void
PlyReader::MessageCallBack
(const std::string& messagePrefix,
 const std::string& fileName,
 std::size_t lineNumber,
 const std::string& message)
{
  std::cout << messagePrefix << ": ";
  std::cout << fileName << "[" << lineNumber << "]: ";
  std::cout << message << std::endl;
}





////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
void
PlyReader::ParseFile
()
{
  std::ifstream ifstream;
  ifstream.open(this->fInputPath.c_str());
  if (!ifstream.is_open())
  {
    std::cerr << "PlyReader: "
    << this->fInputPath
    << ": no such file or directory"
    << std::endl;

    exit(EXIT_FAILURE);
  }

  // prepare ply parser
  ply::ply_parser::flags_type plyParserFlags = 0;
  ply::ply_parser plyParser(plyParserFlags);


  plyParser.info_callback(std::tr1::bind(
        boost::bind(&PlyReader::MessageCallBack,
                    this,
                    std::string("Ply-Info"),
                    this->fInputPath.string(),
                    _1,
                    _2),
        std::tr1::placeholders::_1,
        std::tr1::placeholders::_2));

  plyParser.warning_callback(std::tr1::bind(
   boost::bind(&PlyReader::MessageCallBack,
                    this,
                    std::string("Ply-Warning"),
                    this->fInputPath.string(),
                    _1,
                    _2),
    std::tr1::placeholders::_1,
    std::tr1::placeholders::_2));

  plyParser.error_callback(std::tr1::bind(
    boost::bind(&PlyReader::MessageCallBack,
                    this,
                    std::string("Ply-Error"),
                    this->fInputPath.string(),
                    _1,
                    _2),
    std::tr1::placeholders::_1,
    std::tr1::placeholders::_2));

  // register element definition callback
  plyParser.element_definition_callback(std::tr1::bind(
    &PlyReader::ElementDefinitionCallback,
    this,
    std::tr1::placeholders::_1,
    std::tr1::placeholders::_2));

  // register scalar property definition callbacks
  ply::ply_parser::scalar_property_definition_callbacks_type spdc;
  ply::at<ply::float32>(spdc) = std::tr1::bind(
    &io::PlyReader::ScalarPropertyDefinitionCallback<ply::float32>,
    this,
    std::tr1::placeholders::_1,
    std::tr1::placeholders::_2);
  ply::at<ply::uint8>(spdc) = std::tr1::bind(
    &io::PlyReader::ScalarPropertyDefinitionCallback<ply::uint8>,
    this,
    std::tr1::placeholders::_1,
    std::tr1::placeholders::_2);
  plyParser.scalar_property_definition_callbacks(spdc);

  plyParser.parse(ifstream);
}


} // namespace io
