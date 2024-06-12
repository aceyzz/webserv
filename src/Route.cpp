#include "Route.hpp"

Route::~Route()
{
	if (_cgi != nullptr)
		delete _cgi;
}
