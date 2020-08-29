#include <iostream>
#include <libbear/ea/gene.h>

std::ostream&
libbear::detail::
operator<<(std::ostream& os, const basic_gene& bg)
{
  bg.print(os);
  return os;
}

bool
libbear::detail::
operator==(const basic_gene& l, const basic_gene& r)
{
  return typeid(l) == typeid(r) && l.equal(r);
}

bool
libbear::detail::
operator!=(const basic_gene& l, const basic_gene& r)
{
  return !operator==(l, r);
}

bool
libbear::detail::basic_gene::
equal(const detail::basic_gene&) const
{
  return true;
}
