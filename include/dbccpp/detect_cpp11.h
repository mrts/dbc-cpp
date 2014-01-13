#ifndef DETECTCPP11_H__
#define DETECTCPP11_H__

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L) || defined(_MSC_VER) && _MSC_VER >= 1800
  #define DBCCPP_HAVE_CPP11
#endif

#endif /* DETECTCPP11_H */