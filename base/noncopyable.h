/*
* 从boost中引用
*/
#ifndef _TF_NONCOPYABLE_H_
#define _TF_NONCOPYABLE_H_

namespace noncopyable_  // protection from unintended ADL
{
  class noncopyable
  {
   protected:
      noncopyable() {}
      ~noncopyable() {}
   private:  // emphasize the following members are private
      noncopyable( const noncopyable& );
      const noncopyable& operator=( const noncopyable& );
  };
}

typedef noncopyable_::noncopyable noncopyable;

#endif // _TF_NONCOPYABLE_H_
