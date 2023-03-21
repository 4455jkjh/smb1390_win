#include "Driver.h"

votable* create_votable(const char* name,
	int votable_type,
	int (*callback)(struct votable* votable,
		void* data,
		int effective_result,
		const char* effective_client),
	void* data)
{
	UNREFERENCED_PARAMETER(votable_type);
	UNREFERENCED_PARAMETER(data);
	UNREFERENCED_PARAMETER(callback);
	UNREFERENCED_PARAMETER(name);

}