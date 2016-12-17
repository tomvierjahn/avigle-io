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

#ifndef AVIGLE__IO__INPUT_ADAPTER_INTERFACE_H_
#define AVIGLE__IO__INPUT_ADAPTER_INTERFACE_H_


#include <string>


namespace io
{

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
template <typename FloatType>
class InputAdapterInterface
{
public:
  virtual ~InputAdapterInterface() {}
  virtual void OnBeginPoint() = 0;
  virtual void OnPointPosition(FloatType x, FloatType y, FloatType z) = 0;
  virtual void OnPointNormal(FloatType x, FloatType y, FloatType z) = 0;
  virtual void OnPointColour(FloatType r, FloatType g, FloatType b) = 0;
  virtual void OnPointTexCoord(unsigned int id, FloatType u, FloatType v) = 0;
  virtual void OnEndPoint() = 0;

  virtual void OnTexture(
    unsigned int id,
    const std::string& fileName,
    unsigned int width, unsigned int height,
    FloatType camPosX, FloatType camPosY, FloatType camPosZ,
    FloatType camDirX, FloatType camDirY, FloatType camDirZ,
    FloatType m11, FloatType m12, FloatType m13,
    FloatType m21, FloatType m22, FloatType m23,
    FloatType m31, FloatType m32, FloatType m33,
    FloatType offsetX, FloatType offsetY, FloatType offsetZ,
    FloatType offsetU, FloatType offsetV) = 0;
};  // class


} // namespace io


#endif  // #ifndef AVIGLE__IO__INPUT_ADAPTER_INTERFACE_H_
