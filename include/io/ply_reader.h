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

#ifndef AVIGLE__IO__PLY_READER_H_
#define AVIGLE__IO__PLY_READER_H_


#include <fstream>
#include <iostream>
#include <string>
#include <tr1/functional>
#include <tr1/tuple>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/function.hpp>
#include <boost/tokenizer.hpp>

#include <io/io_api.h>
#include <io/input_adapter_interface.h>
#include <io/reader_tools.h>


namespace io
{

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
class IO_API PlyReader
{
  friend class InputData;

private:
  PlyReader(const std::string& fileName);
  ~PlyReader();

  void ParseFile();

  template <typename FloatType>
  void Load(InputAdapterInterface<FloatType>* pInputAdapter);

  void MessageCallBack(const std::string& messagePrefix,
                       const std::string& fileName,
                       std::size_t lineNumber,
                       const std::string& message);

  typedef std::tr1::tuple<std::tr1::function<void()>,
                          std::tr1::function<void()> > ElementCallbackTuple;
  ElementCallbackTuple ElementDefinitionCallback(
    const std::string& elementName,
    std::size_t count);

  template <typename ScalarType>
  std::tr1::function<void (ScalarType)>
  ScalarPropertyDefinitionCallback(
    const std::string& elementName,
    const std::string& propertyName);

  void VertexBeginCallback();
  void VertexEndCallback();

  boost::filesystem::path fInputPath;

  boost::function<void (void)> fAdapterOnBeginPoint;
  boost::function<void (double, double, double)> fAdapterOnPointPosition;
  boost::function<void (double, double, double)> fAdapterOnPointNormal;
  boost::function<void (double, double, double)> fAdapterOnPointColour;
  boost::function<void (void)> fAdapterOnEndPoint;

  double fPositionX;
  double fPositionY;
  double fPositionZ;
  double fNormalX;
  double fNormalY;
  double fNormalZ;
  double fColourR;
  double fColourG;
  double fColourB;
};  // class





////////////////////////////////////////////////////////////////////////////////
/// Loader
////////////////////////////////////////////////////////////////////////////////
template <typename FloatType>
void
PlyReader::Load
(InputAdapterInterface<FloatType>* pInputAdapter)
{
  this->fAdapterOnBeginPoint =
    boost::bind(&io::InputAdapterInterface<FloatType>::OnBeginPoint,
                pInputAdapter);

  this->fAdapterOnPointPosition =
  boost::bind(&io::InputAdapterInterface<FloatType>::OnPointPosition,
              pInputAdapter, _1, _2, _3);

  this->fAdapterOnPointNormal =
  boost::bind(&io::InputAdapterInterface<FloatType>::OnPointNormal,
              pInputAdapter, _1, _2, _3);

  this->fAdapterOnPointColour =
  boost::bind(&io::InputAdapterInterface<FloatType>::OnPointColour,
              pInputAdapter, _1, _2, _3);

  this->fAdapterOnEndPoint =
    boost::bind(&io::InputAdapterInterface<FloatType>::OnEndPoint,
                pInputAdapter);

  this->ParseFile();
}


} // namespace io


#endif  // #ifdef AVIGLE__IO__PLY_READER_H_
