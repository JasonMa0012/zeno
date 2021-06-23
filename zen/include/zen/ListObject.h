#pragma once

#include <zen/zen.h>
#include <vector>
#include <memory>

namespace zen {

struct ListObject : IObjectClone<ListObject> {
  std::vector<std::shared_ptr<IObject>> arr;
};

}
