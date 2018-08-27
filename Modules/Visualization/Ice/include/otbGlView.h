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

#ifndef otb_GlView_h
#define otb_GlView_h


#include "otbGeoInterface.h"
#include "otbGlActor.h"
#include "otbViewSettings.h"

#include <map>
#include <vector>
#include <cassert>
#include <string>

namespace otb
{

template< typename T >
void
assert_quiet_NaN( T val )
{
  assert( !std::numeric_limits< T >::has_quiet_NaN ||
	  ( std::numeric_limits< T >::has_quiet_NaN &&
	    val!=std::numeric_limits< T >::quiet_NaN() ) );
  // make production build happy
  ((void)val);
}

template< typename T >
void
assert_signaling_NaN( T val )
{
  assert( !std::numeric_limits< T >::has_signaling_NaN ||
	  ( std::numeric_limits< T >::has_signaling_NaN &&
	    val!=std::numeric_limits< T >::signaling_NaN() ) );
  // make production build happy
  ((void)val);
}

template< typename T >
void
assert_NaN( T val )
{
  assert_quiet_NaN( val );
  assert_signaling_NaN( val );
  // make production build happy
  ((void)val);
}

/** 
 * The GlView class acts like an OpenGl scene where actors deriving
 * from the GlActor class can be rendered. The GlView class contains the
 * OpenGl viewport and manages:
 * - The size of the viewport,
 * - The entire refresh loop, including light or heavy rendering of
 * all actors and all OpenGl specific stuff needed before and after
 * the actors update,
 * - The actors stack (order in which actors are rendered).
 * 
 * All parameters related to scene description (origin, spacing, angle
 * ...) are stored and managed by the ViewSettings class.
 */
class OTBIce_EXPORT GlView 
  : public itk::Object
{
public:
  typedef GlView                                          Self;
  typedef itk::Object                                     Superclass;
  typedef itk::SmartPointer<Self>                         Pointer;
  typedef itk::SmartPointer<const Self>                   ConstPointer;

  typedef otb::GlActor                                    ActorType;
  typedef std::map<std::string,ActorType::Pointer>        ActorMapType;
  typedef std::vector<std::string>                        StringVectorType;

  /**
   * Type definition for otb::GlActor storage key.
   */
  typedef StringVectorType::value_type KeyType;

  itkNewMacro(Self);

  /**
   * The Initialize method will reset the OpenGl viewport to the given
   * size, clear view settings and remove any existing actor.
   * \param sx Width of the viewport
   * \param sy Height of the viewport
   */ 
  void Initialize(unsigned int sx, unsigned int sy);

  /**
   * This method allows adding a new actor (deriving from GlActor) to
   * the GlView. The actor can be identified by an optional key. If
   * not provided, and the default value is used, the method will
   * generate a key to identify the actor. In both case, the key is
   * returned by the method.  
   * \param actor The actor to be added
   * \param key The key to be used to identify the actor (default to
   * empty string)
   * \return The key identifying the actor (either passed to the
   * method or generated by it).
   */
  std::string AddActor(ActorType * actor, const std::string & key = "");

  /** 
   * This method will try to remove the actor identified by the given
   * key.
   * \param key The key identifying the actor to remove
   * \return True if the actor has been found and removed, false
   * otherwise
   */
  bool RemoveActor(const std::string & key);

  /**
   * This method will remove all existing actors at once.
   */
  void ClearActors();

  /**
   * This method allows retrieving a pointer to the actor identified
   * by the given key.
   * \param key The key identifying the actor to retrieve
   * \return A pointer to the retrieved actor. This pointer will be
   * null if no actor could be found with this key.
   */  
  ActorType::Pointer GetActor(const std::string & key) const;

  /**
   * Tells whether an otb::GlActor is contained given its storage key.
   *
   * @param key otb::GlActor storage key.
   *
   * @return true if an otb::GlActor is contained given storage key.
   */
  bool ContainsActor( const KeyType & key ) const;

  /**
   * This method will return a vector containing the keys of all
   * actors.
   * \return A vector of string containing the keys of all actors.
   */ 
  std::vector<std::string> GetActorsKeys() const;

  /**
   * This method handles all the things that should be done before
   * rendering.
   */
  void BeforeRendering();

  /**
   * This method handles all the things that should be after before
   * rendering.
   */
  void AfterRendering();

  /**
   * This method will update the rendering of the OpenGl viewport,
   * taking into account all parameters in the ViewSettings, without
   * fetching any missing data from disk or RAM. It is therefore very
   * fast.
   */
  void LightRender();

  /**
   * This method will update the rendering of the OpenGl viewport,
   * taking into account all parameters in the ViewSettings, but will
   * first compute and load any missing data for a complete
   * rendering. As such, this update routine can be time consuming.
   */
  void HeavyRender();

  // Resize viewport
  void Resize(unsigned int sx, unsigned int sy);

  itkSetObjectMacro(Settings,ViewSettings);
  itkGetObjectMacro(Settings,ViewSettings);
  itkGetConstObjectMacro(Settings,ViewSettings);

  //comment this macro (not compiling with OTB 3.X)
  // Get Rendering order
  const StringVectorType & GetRenderingOrder() const
  {
    return m_RenderingOrder;
  };

  /**
   * Arbitrary set the rendering order of some or all of contained
   * otb::GlActor instances.
   *
   * Keys which are not contained will be ignored.
   *
   * @param keys The ordered sequence of keys.
   * @param front <code>true</code> to order selected otb::GlActor
   *              instances in front of non-selected ones.
   */
  void SetRenderingOrder( const StringVectorType & keys,
                          bool front );

  // This will rotate the rendering order (without modifying the order)
  void RotateRenderingOrder(bool down = false);

  // Move actor in rendering order
  void MoveActorInRenderingOrder(std::string key, bool down = false);

  // Move actor to the end of rendering order (either front if front
  // is set to true or back if front is set to false)
  void MoveActorToEndOfRenderingOrder(std::string key, bool front = false);

  /**
   * Reproject point and spacing expressed in viewport coordinate
   * system into given actor coordinate system.
   */
  template< typename P, typename S, typename P2, typename S2 >
  bool ReprojectFromView( P & center,
			  S & spacing,
			  const KeyType & key,
			  const P2 & vcenter,
			  const S2 & vspacing,
			  double norm = 1000.0 ) const;

  /**
   */
  template< typename P >
  size_t GetExtent( P & origin, P & extent, bool isOverlay =false ) const;

  /**
   */
  template< typename Point, typename Spacing >
  bool ZoomToExtent( const Spacing & native,
		     Point & center,
		     Spacing & spacing ) const;

  /**
   */
  template< typename Point, typename Spacing >
  bool ZoomToLayer( const KeyType & key,
		    const Spacing & native,
		    Point & center,
		    Spacing & spacing ) const;

  /**
   */
  template< typename Point, typename Spacing >
  bool ZoomToRegion( const Point & origin,
		     const Point & extent,
		     const Spacing & native,
		     Point & center,
		     Spacing & spacing ) const;

  /**
   */
  template< typename Point, typename Spacing >
  bool ZoomToFull( const KeyType & key,
		   Point & center,
		   Spacing & spacing,
		   double units = 1000.0 ) const;

  /**
   */
  void SaveScreenshot( const std::string & filename ) const;

protected:
  GlView();

  ~GlView() override;

private:
  // prevent implementation
  GlView(const Self&);
  void operator=(const Self&);

  ViewSettings::Pointer m_Settings;

  ActorMapType          m_Actors;

  StringVectorType      m_RenderingOrder;

}; // End class GlView


template< typename P, typename S, typename P2, typename S2 >
bool
GlView
::ReprojectFromView( P & center,
		     S & spacing,
		     const KeyType & key,
		     const P2 & vcenter,
		     const S2 & vspacing,
		     double norm ) const
{
  // std::cout << "otb::GlView@" << std::hex << this << std::endl << "{" << std::endl;

  assert_NaN( vcenter[ 0 ] );
  assert_NaN( vcenter[ 1 ] );

  assert_NaN( vspacing[ 0 ] );
  assert_NaN( vspacing[ 1 ] );


  //
  // Reference actor has not been found.
  otb::GlActor::Pointer actor( GetActor( key ) );

  if( actor.IsNull() )
    return false;

  //
  // Reference actor does not implement geo-interface.
  const otb::GeoInterface * geo =
    dynamic_cast< const GeoInterface * >( actor.GetPointer() );

  if( geo==nullptr )
    return false;

  const otb::GeoInterface::Spacing2 nativeReferenceSpacing = geo->GetSpacing();
  
  //
  // Compute transform origin.
  if( !geo->TransformFromViewport( center, vcenter, true ) )
    return false;

  //
  // Compute transformed X-axis extremity.
  GeoInterface::Point2d x( vcenter );

  x[ 0 ] += norm * vspacing[ 0 ]; 

  // std::cout << "X {" << std::endl;

  if( !geo->TransformFromViewport( x, x, true ) )
    return false;

  // std::cout << "x: " << x[ 0 ] << ", " << x[ 1 ] << std::endl;

  // assert_NaN( x[ 0 ] );
  // assert_NaN( x[ 1 ] );

  // std::cout << "}" << std::endl;

  //
  // Compute transformed Y-axis extremity.
  GeoInterface::Point2d y( vcenter );

  y[ 1 ] += norm * vspacing[ 1 ];

  // std::cout << "Y {" << std::endl;

  if( !geo->TransformFromViewport( y, y, true ) )
    return false; 

  // std::cout << "y: " << y[ 0 ] << ", " << y[ 1 ] << std::endl;

  // assert_NaN( y[ 0 ] );
  // assert_NaN( y[ 1 ] );

  // std::cout << "}" << std::endl;

  //
  // Compute transformed spacing.
  //
  // Note SAT:
  //     Formula has been taken from IceViewer::key_callback(). I think
  // that the norm of the transformed X and Y axises is not
  // the new spacing if transform contains a rotation.
  //     To correct this, transformed X and Y vectors should be
  // projected against reference actor X and Y axises (using vectorial
  // dot product).

  x[ 0 ] -= center[ 0 ];
  x[ 1 ] -= center[ 1 ];

  y[ 0 ] -= center[ 0 ];
  y[ 1 ] -= center[ 1 ];

  spacing[ 0 ] = std::sqrt( x[ 0 ] * x[ 0 ] + x[ 1 ] * x[ 1 ] ) / norm;
  spacing[ 1 ] = std::sqrt( y[ 0 ] * y[ 0 ] + y[ 1 ] * y[ 1 ] ) / norm;

  // New spacing signs should match signs of the reference image spacing
  
  if( nativeReferenceSpacing[0]<0.0 )
    spacing[ 0 ] = -spacing[ 0 ];

  if( nativeReferenceSpacing[1]<0.0 )
    spacing[ 1 ] = -spacing[ 1 ];

  //
  // Chech outputs.
  assert_NaN( center[ 0 ] );
  assert_NaN( center[ 1 ] );

  assert_NaN( spacing[ 0 ] );
  assert_NaN( spacing[ 1 ] );

  // std::cout << "} otb::GlView@" << std::hex << this << std::endl;

  //
  // Ok.
  return true;
}


template< typename P >
size_t
GlView
::GetExtent( P & origin, P & extent, bool isOverlay ) const
{
  if( m_Actors.empty() )
    {
    origin.Fill( 0 );
    extent.Fill( 0 );

    return 0;
    }


  origin[ 0 ] = std::numeric_limits< typename P::ValueType >::infinity();
  origin[ 1 ] = std::numeric_limits< typename P::ValueType >::infinity();

  extent[ 0 ] = -std::numeric_limits< typename P::ValueType >::infinity();
  extent[ 1 ] = -std::numeric_limits< typename P::ValueType >::infinity();

  size_t count = 0;

  for( ActorMapType::const_iterator it( m_Actors.begin() );
       it!=m_Actors.end();
       ++it )
    {
    assert( !it->second.IsNull() );

    if( it->second->GetOverlay()==isOverlay )
      {
      P o;
      P e;

      o.Fill( 0 );
      e.Fill( 0 );

      it->second->GetExtent( o[ 0 ], o[ 1 ], e[ 0 ], e[ 1 ] );


      if( o[ 0 ]<origin[ 0 ] )
	origin[ 0 ] = o[ 0 ];

      if( o[ 1 ]<origin[ 1 ] )
	origin[ 1 ] = o[ 1 ];

      if( o[ 0 ]>extent[ 0 ] )
	extent[ 0 ] = o[ 0 ];

      if( o[ 1 ]>extent[ 1 ] )
	extent[ 1 ] = o[ 1 ];


      if( e[ 0 ]<origin[ 0 ] )
	origin[ 0 ] = e[ 0 ];

      if( e[ 1 ]<origin[ 1 ] )
	origin[ 1 ] = e[ 1 ];

      if( e[ 0 ]>extent[ 0 ] )
	extent[ 0 ] = e[ 0 ];

      if( e[ 1 ]>extent[ 1 ] )
	extent[ 1 ] = e[ 1 ];

      ++ count;
      }
    }

  if( count==0 )
    {
    origin.Fill( 0 );
    extent.Fill( 0 );
    }

  return count;
}


template< typename Point, typename Spacing >
bool
GlView
::ZoomToExtent( const Spacing & native, Point & center, Spacing & spacing ) const
{
  Point o;
  Point e;

  o.Fill( 0 );
  e.Fill( 0 );

  // Get origin and extent of all layers in viewport system.
  if( GetExtent( o, e )==0 )
    return false;

  // std::cout << "origin: [ " << o[ 0 ] << ", " << o[ 1 ] << " ]" << std::endl;
  // std::cout << "extent: [ " << e[ 0 ] << ", " << e[ 1 ] << " ]" << std::endl;

  // Zoom to overall region.
  return ZoomToRegion( o, e, native, center, spacing );
}


template< typename Point, typename Spacing >
bool
GlView
::ZoomToLayer( const KeyType & key,
	       const Spacing & native,
	       Point & center,
	       Spacing & spacing ) const
{
  Point o;
  Point e;

  // Get layer actor.
  GlActor::Pointer actor( GetActor( key ) );

  // If not found...
  if( actor.IsNull() )
    return false;

  // Get origin and extent of layer.
  actor->GetExtent( o[ 0 ], o[ 1 ], e[ 0 ], e[ 1 ] );

  // Zoom layer region.
  return ZoomToRegion( o, e, native, center, spacing );
}


template< typename Point, typename Spacing >
bool
GlView
::ZoomToRegion( const Point & origin,
		const Point & extent,
		const Spacing & native,
		Point & center,
		Spacing & spacing ) const
{
  // std::cout
  //   << std::hex << this << std::dec
  //   << "::ZoomToRegion( "
  //   << "[" << origin[ 0 ] << ", " << origin[ 1 ] << "], "
  //   << "[" << extent[ 0 ] << ", " << extent[ 1 ] << "], "
  //   << "[" << native[ 0 ] << ", " << native[ 1 ] << "] )"
  //   << std::endl;

  // Compute center point.
  center.SetToMidPoint( origin, extent );

  // std::cout << "-> center: " << center[ 0 ] << ", " << center[ 1 ] << std::endl;

  // Get scale of (o, e) in viewport.
  assert( !m_Settings.IsNull() );
  double scale = m_Settings->GetScale( origin, extent, true );

  // std::cout << "-> scale: " << scale << std::endl;

  /*
  assert( !std::numeric_limits< typename Spacing::ValueType >::has_quiet_NaN() ||
	  ( std::numeric_limits< typename Spacing::ValueType >::has_quiet_NaN() &&
	    native[ 0 ]!=std::numeric_limits< typename Spacing::ValueType >::quiet_NaN()
	  )
  );
  assert( !std::numeric_limits< typename Spacing::ValueType >::has_quiet_NaN() ||
	  ( std::numeric_limits< typename Spacing::ValueType >::has_quiet_NaN() &&
	  native[ 1 ]!=std::numeric_limits< typename Spacing::ValueType >::quiet_NaN()
	  )
  );

  assert( !std::numeric_limits< typename Spacing::ValueType >::has_quiet_NaN() ||
	  ( std::numeric_limits< typename Spacing::ValueType >::has_quiet_NaN() &&
	    native[ 0 ]!=std::numeric_limits< typename Spacing::ValueType >::quiet_NaN()
	  )
  );
  assert( !std::numeric_limits< typename Spacing::ValueType >::has_quiet_NaN() ||
	  ( std::numeric_limits< typename Spacing::ValueType >::has_quiet_NaN() &&
	  native[ 1 ]!=std::numeric_limits< typename Spacing::ValueType >::quiet_NaN()
	  )
  );
  */

  // Apply signed scale.
  spacing[ 0 ] = ( native[ 0 ]<0.0 ? -1 : +1 ) * scale;
  spacing[ 1 ] = ( native[ 1 ]<0.0 ? -1 : +1 ) * scale;

  // std::cout << "-> spacing: " << spacing[ 0 ] << ", " << spacing[ 1 ] << std::endl;

  // Ok.
  return true;
}


template< typename Point, typename Spacing >
bool
GlView
::ZoomToFull( const KeyType & key,
	      Point & center,
	      Spacing & spacing,
	      double units ) const
{
  // std::cout
  //   << std::hex << this << std::dec
  //   << "::ZoomToFull();"
  //   << std::endl;

  // Get layer actor.
  GlActor::Pointer actor( GetActor( key ) );

  // If not found...
  if( actor.IsNull() )
    return false;

  // Get geo-interface.
  const GeoInterface * geo =
    dynamic_cast< const GeoInterface * >( actor.GetPointer() );

  if( geo==nullptr )
    return false;

  // Get viewport current center and spacing.
  assert( !m_Settings.IsNull() );

  center = m_Settings->GetViewportCenter();
  spacing = m_Settings->GetSpacing();

  // std::cout << "-> spacing: " << spacing[ 0 ] << ", " << spacing[ 1 ] << std::endl;

  // Get native spacing.
  GeoInterface::Spacing2 n_spacing( geo->GetSpacing() );

  // std::cout << "-> n_spacing: " << n_spacing[ 0 ] << ", " << n_spacing[ 1 ] << std::endl;

  // Transform center point to image space.
  Point o;

  if( !geo->TransformFromViewport( o, center, true ) )
    return false;

  //
  // Consider arbitrary point on the X-axis.
  Point e;

  e[ 0 ] = center[ 0 ] + units * spacing[ 0 ];
  e[ 1 ] = center[ 1 ];

  // Transform considered point.
  if( !geo->TransformFromViewport( e, e, true ) )
    return false;

  // Compute extent vector.
  e[ 0 ] -= o[ 0 ];
  e[ 1 ] -= o[ 1 ];

  // Apply extent vector length to view spacing.
  //
  // MANTIS-1178: Length of vector e must be divided by native
  // spacing.
  //
  // MANTIS-1203: absolute value of native spacing should be
  // considered (to avoid flipping effects).
  spacing[ 0 ] =
    std::abs( n_spacing[ 0 ] ) * units * spacing[ 0 ] /
    std::sqrt( e[ 0 ] * e[ 0 ] + e[ 1 ] * e[ 1 ] );

  //
  // Consider arbitrary point on the Y-axis.
  e[ 0 ] = center[ 0 ];
  e[ 1 ] = center[ 1 ] + units * spacing[ 1 ];

  // Transform considered point.
  if( !geo->TransformFromViewport( e, e, true ) )
    return false;

  // Compute extent vector.
  e[ 0 ] -= o[ 0 ];
  e[ 1 ] -= o[ 1 ];

  // Apply extent vector length to view spacing.
  //
  // MANTIS-1178: Length of vector e must be divided by native
  // spacing.
  //
  // MANTIS-1203: absolute value of native spacing should be
  // considered (to avoid flipping effects).
  spacing[ 1 ] =
    std::abs( n_spacing[ 1 ] ) * units * spacing[ 1 ] /
    std::sqrt( e[ 0 ] * e[ 0 ] + e[ 1 ] * e[ 1 ] );

  // std::cout << "-> spacing: " << spacing[ 0 ] << ", " << spacing[ 1 ] << std::endl;

  //
  // Compute aspect-ratio corrected spacing (smallest pixel is chosen
  // as 1:1 reference).
  //
  // MANTIS-1202
  //
  // MANTIS-1203: restore sign of axis when applying isotrop spacing.
  // {
  if( std::abs( spacing[ 0 ] ) < std::abs( spacing[ 1 ] ) )
    spacing[ 1 ] = ( spacing[ 1 ]<0.0 ? -1 : +1 ) * std::abs( spacing[ 0 ] );
  else
    spacing[ 0 ] = ( spacing[ 0 ]<0.0 ? -1 : +1 ) * std::abs( spacing[ 1 ] );
  // }
  // MANTIS-1202

  // std::cout << "-> spacing: " << spacing[ 0 ] << ", " << spacing[ 1 ] << std::endl;

  //
  // Ok.
  return true;
}

} // End namespace otb

#endif
