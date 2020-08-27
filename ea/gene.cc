#include <iostream>
#include <libbear/ea/gene.h>

std::ostream&
libbear::
operator<<(std::ostream& os, const basic_gene& bg)
{
  bg.print(os);
  return os;
}

bool
libbear::
operator==(const basic_gene& l, const basic_gene& r)
{
  return typeid(l) == typeid(r) && l.equal(r);
}

bool
libbear::
operator!=(const basic_gene& l, const basic_gene& r)
{
  return !operator==(l, r);
}

bool
libbear::basic_gene::
equal(const basic_gene&) const
{
  return true;
}
