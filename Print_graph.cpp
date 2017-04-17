/*  To Compile:
 *    g++ -std=c++11 -o test.bin  print.cpp
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>



struct Interface {
  
  float The_formula( float x ) {
    // Put Your Formula Here:
    x -= 1.0f;
    if( x < 0 )
      x *= -1;
    float y = log10( x + x );
    if( y < -0.75f )
      y = -0.75f;
    return y;
  }
  // Tweak These Values:
  const unsigned int display = 40;
  const unsigned int samples = 100;
  const float x_axis_start = -1.0f;
  const float x_axis_end = 2.0f;
};



//###########################################################################################



using POINT = std::pair< float, float >;
using DATA = std::vector< std::pair< float, float > >;
using PLANE = std::vector< std::string >;



float
Get_progress( const float _a, const float _b, const float _p ) { 
  if( _a < _b ) {
    const float range = _b - _a;
    return ( _p - _a ) / range;
  }
  if( _b < _a ) {
    const float range = _a - _b;
    return 1 - (( _p - _b ) / range );  
  }
  return 0.0f;
}

float
Set_progress( const float _a, const float _b, const float _p ) {
  if( _a < _b ) {
    const float range = _b - _a;
    return ( range * _p ) + _a;
  }
  if( _b < _a ) {
    const float range = _a - _b;
    return _a - ( range * _p );
  }
  return 0.0f;
}



bool
Find_max_min_y( const POINT& _a, const POINT& _b ) {
  return _a.second < _b.second;
}

POINT
Max_min_x( const DATA& _data ) {
  auto find_x = std::minmax_element( _data.begin(), _data.end());
  return { (find_x.first)->first, (find_x.second)->first };  
}

POINT
Max_min_y( const DATA& _data ) {
  auto find_y = std::minmax_element( _data.begin(), _data.end(), &Find_max_min_y );
  return { (find_y.first)->second, (find_y.second)->second };
}

POINT
Find_axes( const POINT& _range_x, const POINT& _range_y, const unsigned int _display ) {
  float progress = Get_progress( _range_x.first, _range_x.second, 0.0f );
  int x_axis = 0.5f + Set_progress( 0.0f, _display, progress );
  progress = Get_progress( _range_y.first, _range_y.second, 0.0f );
  int y_axis = 0.5f + Set_progress( 0.0f, _display, progress );
  if( x_axis < 0 || x_axis >= _display )
    x_axis = -1;
  if( y_axis < 0 || y_axis >= _display )
    y_axis = -1;
  return { x_axis, y_axis };
}



void
Scale_data( DATA& _data, const POINT& _range_x, const POINT& _range_y, const unsigned int _display ) {
  for( unsigned int i = 0 ; i < _data.size() ; ++i ) {
     float progress = Get_progress( _range_x.first, _range_x.second, _data[i].first );
     int x = 0.5f + Set_progress( 0.0f, _display, progress );
     progress = Get_progress( _range_y.first, _range_y.second, _data[i].second );
     int y = 0.5f + Set_progress( 0.0f, _display, progress );
     _data[i].first = x;
     _data[i].second = y;
  }//for
}



void
Draw( const DATA& _data, PLANE& _plane, const POINT& _axes_pos, const unsigned int _display ) {
  // Draw Axes:
  if( _axes_pos.first != -1 )
    for( unsigned int i = 0 ; i < _plane.size() ; ++i ) 
      _plane[i].at(( _axes_pos.first * 2 ) + 1 ) = '|'; 
  if( _axes_pos.second != -1 )
    for( unsigned int i = 1 ; i < _plane[_axes_pos.second].size() ; i += 2 )
      _plane[_axes_pos.second].at( i ) = '-';
  if(( _axes_pos.first != -1 )&&( _axes_pos.second != -1 ))
    _plane[_axes_pos.second].at(( _axes_pos.first * 2 ) + 1 ) = '+';
    
  // Draw Data:
  for( unsigned int i = 0 ; i < _data.size() ; ++i ) {
    _plane.at( _data[i].second ).at(( _data[i].first * 2 ) + 1 ) = '#';  
  }//for
}

void
Print( const PLANE& _plane, const POINT& _range_x, const POINT& _range_y, const POINT& _axes_pos ) {
  // Top:
  std::string top( _plane.size() * 2, ' ' );
  for( unsigned int i = 1 ; i < _plane.size() * 2 ; i += 2 )
    top.at( i ) = '-';
  if( _axes_pos.first != -1 )
    top.at(( _axes_pos.first * 2 ) + 1 ) = '^';
  std::cout <<" O"<< top <<" O\n";

  // The Graph And, y Max/Min:
  for( auto itr = _plane.begin() ; itr != _plane.end() ; ++itr ) {
    std::string line( " |" + *itr + " | " );
    if( std::distance( _plane.begin(), itr ) == _axes_pos.second ) {
      line.at( 1 ) = '<';
      line.at( line.size() - 2 ) = '>';
    }    
    if( itr == _plane.begin())
      line += std::to_string( _range_y.first );  
    else if( itr == ( _plane.end() - 1 ))
      line += std::to_string( _range_y.second );
    std::cout << line <<"\n";
  }//for 
  
  // Bottom:
  std::string bottom( _plane.size() * 2, ' ' );
  for( unsigned int i = 1 ; i < _plane.size() * 2 ; i += 2 )
    bottom.at( i ) = '-';
  if( _axes_pos.first != -1 )
    bottom.at(( _axes_pos.first * 2 ) + 1 ) = 'v';
  std::cout <<" O"<< bottom <<" O\n";

  // x Max/Min: 
  std::string x1 = std::to_string( _range_x.first );
  std::string x2(( _plane.size() * 2 ) - x1.size() - 2, ' ' );
  x1 += x2;
  x1 += std::to_string( _range_x.second );
  std::cout <<"   "<< x1 <<"\n";
}


  
int
main() {
  Interface interface;
  
  // Make Plane:
  std::string str(( interface.display + 1 ) * 2, ' ' );
  for( int i = 0 ; i < str.size() ; i += 2 )
    str[i] = ' ';
  std::vector< std::string > plane( interface.display + 1, str );

  // Make Data:
  std::vector< std::pair< float, float > > data;
  data.reserve( interface.samples );
  for( unsigned int i = 0 ; i < interface.samples ; ++i ) {
    float progress = Get_progress( 0, interface.samples - 1, i );
    float x = Set_progress( interface.x_axis_start, interface.x_axis_end, progress );
    float y = interface.The_formula( x );
    data.push_back({ x, y });    
  }//for

  POINT range_x = Max_min_x( data );
  POINT range_y = Max_min_y( data );
  POINT axes_pos = Find_axes( range_x, range_y, interface.display );
  // Scale Data For Display:
  Scale_data( data, range_x, range_y, interface.display );

  // Draw Data To Plane:
  Draw( data, plane, axes_pos, interface.display );

  // Print Plane In Terminal:
  Print( plane, range_x, range_y, axes_pos );
  
  return 0;
}
