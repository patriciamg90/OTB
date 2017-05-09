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

#include "otbTestMain.h"

void RegisterTests()
{
  REGISTER_TEST(otbVectorDataToLabelMapFilter);
  REGISTER_TEST(otbOGRDataSourceToLabelImageFilterNew);
  REGISTER_TEST(otbOGRDataSourceToLabelImageFilter);
  REGISTER_TEST(otbVectorDataToLabelMapFilterNew);
  REGISTER_TEST(otbLabelImageToVectorDataFilterNew);
  REGISTER_TEST(otbLabelImageToVectorDataFilter);
  REGISTER_TEST(otbLabelImageToOGRDataSourceFilterNew);
  REGISTER_TEST(otbLabelImageToOGRDataSourceFilter);
  REGISTER_TEST(otbVectorDataToLabelImageFilterNew);
  REGISTER_TEST(otbVectorDataToLabelImageFilter);
  REGISTER_TEST(otbPolygonizationRasterizationTest);
  REGISTER_TEST(otbVectorDataRasterizeFilterNew);
  REGISTER_TEST(otbVectorDataRasterizeFilter);
  REGISTER_TEST(otbLabelImageRegionPruningFilter);
  REGISTER_TEST(otbLabelImageRegionMergingFilter);
  REGISTER_TEST(otbLabelMapToVectorDataFilter);
  REGISTER_TEST(otbLabelMapToVectorDataFilterNew);
}
