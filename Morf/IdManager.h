#pragma once

#include <QList>
#include <QSet>
#include <cassert>

namespace Morf
{
  
  //
  template<typename _id_type>
  class IdManager
  {
  public:
    
    //
    typedef _id_type id_t;
    
  public:
    
    //
    IdManager ( id_t init_size = 32 )
      : m_IdSize( 0 )
    {
      reserve( init_size );
    }
    
    //
    id_t allocate (bool auto_grow = true)
    {
      if ( m_FreeList.isEmpty() )
      {
        if ( auto_grow )
          reserve( m_IdSize * 2 );
        else
          return 0;
      }
      
      // Get ID from the tail of the free list
      id_t new_id = m_FreeList.last();
      
      // Pop out the tail of the free list
      m_FreeList.removeLast();
      
      // Add to the active set
      m_ActiveSet.insert( new_id );
      
      return new_id;
    }
    
    //
    void release ( id_t idx )
    {
      if ( m_ActiveSet.contains( idx ) )
      {
        // Remove from the active set
        m_ActiveSet.remove( idx );
        
        // Add to the free list
        m_FreeList.append( idx );
        
      }
    }
    
    //
    void reserve ( id_t size )
    {
      // Assertion
      assert( size > 0 );
      
      // Check if size > m_IdSize
      if ( size > m_IdSize )
      {
        // Expand Size
        for ( id_t i = m_IdSize + 1 ; i <= size; ++i )
        {
          m_FreeList.append( i );
        }
        
        m_IdSize = size;
      }
    }
    
  private:
    
    //
    id_t           m_IdSize;
    
    //
    QList<id_t>    m_FreeList;
    
    //
    QSet<id_t>     m_ActiveSet;
    
  };
  
}
