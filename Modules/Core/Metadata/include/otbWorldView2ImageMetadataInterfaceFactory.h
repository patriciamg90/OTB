/*
 * Copyright (C) 2005-2017 Centre National d'Etudes Spatiales (CNES)
 *
 * This file is part of Orfeo Toolbox
 *
 *     https://www.orfeo-toolbox.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef otbWorldView2ImageMetadataInterfaceFactory_h
#define otbWorldView2ImageMetadataInterfaceFactory_h

#include "itkObjectFactoryBase.h"
#include "OTBMetadataExport.h"

namespace otb
{
/** \class WorldView2ImageMetadataInterfaceFactory
 * \brief Creating an instance of a ImageMetadataInterface object using object factory.
 *
 * \ingroup OTBMetadata
 */
class OTBMetadata_EXPORT WorldView2ImageMetadataInterfaceFactory : public itk::ObjectFactoryBase
{
public:
  /** Standard class typedefs. */
  typedef WorldView2ImageMetadataInterfaceFactory Self;
  typedef itk::ObjectFactoryBase                  Superclass;
  typedef itk::SmartPointer<Self>                 Pointer;
  typedef itk::SmartPointer<const Self>           ConstPointer;

  /** Class methods used to interface with the registered factories. */
  const char* GetITKSourceVersion(void) const ITK_OVERRIDE;
  const char* GetDescription(void) const ITK_OVERRIDE;

  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(WorldView2ImageMetadataInterfaceFactory, itk::ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory(void)
  {
    WorldView2ImageMetadataInterfaceFactory::Pointer worldView2IMIFactory =
      WorldView2ImageMetadataInterfaceFactory::New();
    itk::ObjectFactoryBase::RegisterFactory(worldView2IMIFactory);
  }

protected:
  WorldView2ImageMetadataInterfaceFactory();
  ~WorldView2ImageMetadataInterfaceFactory() ITK_OVERRIDE;

private:
  WorldView2ImageMetadataInterfaceFactory(const Self &); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

};

} // end namespace otb

#endif
