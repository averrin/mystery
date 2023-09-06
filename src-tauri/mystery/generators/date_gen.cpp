#include "../include/generators/date_gen.hpp"
#include <cassert>
#include <stdexcept>

std::shared_ptr<Dates> DateGenerator::get(DatesSpec spec) {
  auto d = std::make_shared<Dates>();
  d->day = spec.day.get();
  d->month = spec.month.get();

  if (spec.year.value) {
    d->year = spec.year.get();
  } else if (spec.age.value) {
    d->year = this->rm->currentYear - spec.age.get();
  } else {
    d->year = spec.year.get_or(
        intSpec{std::pair{this->rm->currentYear - 100, this->rm->currentYear}});
  }
  if (d->year < 1850 || d->year > this->rm->currentYear) {
    throw std::runtime_error("broken year");
  }
  return this->get(this->save(d));
}

std::optional<std::shared_ptr<Dates>> DateGenerator::find(DatesSpec) {}
std::vector<std::shared_ptr<Dates>> DateGenerator::findAll(DatesSpec) {}
