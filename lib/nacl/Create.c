#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Error.h"
#include "DataType.h"
#include "NDArray.h"
#include "Commands.h"
#include "Interfaces.h"
#include "Message.h"
#include "Strings.h"
#include "IdMap.h"
#include "Util.h"

typedef void (*StepInitFunction)(int32_t, double, double, void*);
static void initLinearF32(int32_t samples, double start, double step, float dataOut[restrict static samples]);
static void initLinearF64(int32_t samples, double start, double step, double dataOut[restrict static samples]);

static const StepInitFunction stepInitFunctions[] = {
	[FJS_DataType_F64] = (StepInitFunction) initLinearF64,
	[FJS_DataType_F32] = (StepInitFunction) initLinearF32
};

enum FJS_Create_Argument {
	FJS_Create_Argument_Out,
	FJS_Create_Argument_DataType,
	FJS_Create_Argument_Shape
};

static const struct FJS_VariableDescriptor createDescriptors[] =
{
	[FJS_Create_Argument_Out] = { 
		.type = FJS_VariableType_Int32,
		.name = FJS_StringVariable_Out
	},
	[FJS_Create_Argument_DataType] = {
		.type = FJS_VariableType_DataType,
		.name = FJS_StringVariable_Datatype
	},
	[FJS_Create_Argument_Shape] = {
		.type = FJS_VariableType_Buffer,
		.name = FJS_StringVariable_Shape
	}
};

void FJS_Parse_Create(PP_Instance instance, struct PP_Var message) {
	struct FJS_Variable variables[FJS_COUNT_OF(createDescriptors)];
	enum FJS_Error error = FJS_Error_Ok;

	error = FJS_Message_Parse(FJS_COUNT_OF(createDescriptors), createDescriptors, variables, message);
	if (error != FJS_Error_Ok) {
		FJS_LOG_ERROR("Parse error");
		goto cleanup;
	}

	error = FJS_Execute_Create(instance,
		variables[FJS_Create_Argument_Out].parsedValue.asInt32,
		variables[FJS_Create_Argument_Shape].parsedValue.asBuffer.size / 4,
		variables[FJS_Create_Argument_Shape].parsedValue.asBuffer.pointer,
		variables[FJS_Create_Argument_DataType].parsedValue.asDatatype);
	if (!FJS_Message_SetStatus(instance, FJS_ResponseVariable, error)) {
		goto cleanup;
	}

	messagingInterface->PostMessage(instance, FJS_ResponseVariable);

	FJS_Message_RemoveStatus(FJS_ResponseVariable);
cleanup:
	FJS_Message_FreeVariables(FJS_COUNT_OF(variables), variables);
}

enum FJS_CreateFromBuffer_Argument {
	FJS_CreateFromBuffer_Argument_Out,
	FJS_CreateFromBuffer_Argument_DataType,
	FJS_CreateFromBuffer_Argument_Shape,
	FJS_CreateFromBuffer_Argument_Buffer
};

static const struct FJS_VariableDescriptor createFromBufferDescriptors[] =
{
	[FJS_CreateFromBuffer_Argument_Out] = { 
		.type = FJS_VariableType_Int32,
		.name = FJS_StringVariable_Out
	},
	[FJS_CreateFromBuffer_Argument_DataType] = {
		.type = FJS_VariableType_DataType,
		.name = FJS_StringVariable_Datatype
	},
	[FJS_CreateFromBuffer_Argument_Shape] = {
		.type = FJS_VariableType_Buffer,
		.name = FJS_StringVariable_Shape
	},
	[FJS_CreateFromBuffer_Argument_Buffer] = {
		.type = FJS_VariableType_Buffer,
		.name = FJS_StringVariable_Buffer
	}
};

void FJS_Parse_CreateFromBuffer(PP_Instance instance, struct PP_Var message) {
	struct FJS_Variable variables[FJS_COUNT_OF(createFromBufferDescriptors)];
	enum FJS_Error error = FJS_Error_Ok;

	error = FJS_Message_Parse(FJS_COUNT_OF(createFromBufferDescriptors), createFromBufferDescriptors, variables, message);
	if (error != FJS_Error_Ok) {
		FJS_LOG_ERROR("Parse error");
		goto cleanup;
	}

	error = FJS_Execute_CreateFromBuffer(instance,
		variables[FJS_CreateFromBuffer_Argument_Out].parsedValue.asInt32,
		variables[FJS_CreateFromBuffer_Argument_Shape].parsedValue.asBuffer.size / 4,
		variables[FJS_CreateFromBuffer_Argument_Shape].parsedValue.asBuffer.pointer,
		variables[FJS_CreateFromBuffer_Argument_DataType].parsedValue.asDatatype,
		variables[FJS_CreateFromBuffer_Argument_Buffer].parsedValue.asBuffer.size,
		variables[FJS_CreateFromBuffer_Argument_Buffer].parsedValue.asBuffer.pointer);
	if (!FJS_Message_SetStatus(instance, FJS_ResponseVariable, error)) {
		goto cleanup;
	}

	messagingInterface->PostMessage(instance, FJS_ResponseVariable);

	FJS_Message_RemoveStatus(FJS_ResponseVariable);
cleanup:
	FJS_Message_FreeVariables(FJS_COUNT_OF(variables), variables);
}

enum FJS_LinSpace_Argument {
	FJS_LinSpace_Argument_Out,
	FJS_LinSpace_Argument_Start,
	FJS_LinSpace_Argument_Stop,
	FJS_LinSpace_Argument_Samples,
	FJS_LinSpace_Argument_Closed,
	FJS_LinSpace_Argument_DataType,
};

static const struct FJS_VariableDescriptor linspaceDescriptors[] =
{
	[FJS_LinSpace_Argument_Out] = { 
		.type = FJS_VariableType_Int32,
		.name = FJS_StringVariable_Out
	},
	[FJS_LinSpace_Argument_Start] = { 
		.type = FJS_VariableType_Float64,
		.name = FJS_StringVariable_Start
	},
	[FJS_LinSpace_Argument_Stop] = {
		.type = FJS_VariableType_Float64,
		.name = FJS_StringVariable_Stop
	},
	[FJS_LinSpace_Argument_Samples] = {
		.type = FJS_VariableType_Int32,
		.name = FJS_StringVariable_Samples
	},
	[FJS_LinSpace_Argument_Closed] = {
		.type = FJS_VariableType_Boolean,
		.name = FJS_StringVariable_Closed
	},
	[FJS_LinSpace_Argument_DataType] = {
		.type = FJS_VariableType_DataType,
		.name = FJS_StringVariable_Datatype
	}
};

void FJS_Parse_LinSpace(PP_Instance instance, struct PP_Var message) {
	struct FJS_Variable variables[FJS_COUNT_OF(linspaceDescriptors)];
	enum FJS_Error error = FJS_Error_Ok;

	error = FJS_Message_Parse(FJS_COUNT_OF(linspaceDescriptors), linspaceDescriptors, variables, message);
	if (error != FJS_Error_Ok) {
		FJS_LOG_ERROR("Parse error");
		goto cleanup;
	}

	error = FJS_Execute_LinSpace(instance,
		variables[FJS_LinSpace_Argument_Out].parsedValue.asInt32,
		variables[FJS_LinSpace_Argument_Start].parsedValue.asFloat64,
		variables[FJS_LinSpace_Argument_Stop].parsedValue.asFloat64,
		variables[FJS_LinSpace_Argument_Samples].parsedValue.asInt32,
		variables[FJS_LinSpace_Argument_Closed].parsedValue.asBoolean,
		variables[FJS_LinSpace_Argument_DataType].parsedValue.asDatatype);
	if (!FJS_Message_SetStatus(instance, FJS_ResponseVariable, error)) {
		goto cleanup;
	}

	messagingInterface->PostMessage(instance, FJS_ResponseVariable);

	FJS_Message_RemoveStatus(FJS_ResponseVariable);
cleanup:
	FJS_Message_FreeVariables(FJS_COUNT_OF(variables), variables);
}

enum FJS_Error FJS_Execute_Create(PP_Instance instance, int32_t idOut, size_t dimensions, uint32_t shape[static dimensions], enum FJS_DataType datatype) {
	if (dimensions == 0) {
		return FJS_Error_EmptyShape;
	}
	const uint32_t elementSize = FJS_DataType_GetSize(datatype);
	if (elementSize == 0) {
		return FJS_Error_InvalidDataType;
	}
	uint32_t length = 1;
	for (uint32_t dimension = 0; dimension < dimensions; dimension++) {
		const uint32_t measure = shape[dimension];
		if (measure < 1) {
			return FJS_Error_DegenerateShape;
		}
		/* This multiplication can easily overflow */
		if (!FJS_Util_Mul32u(length, measure, &length)) {
			return FJS_Error_LengthOverflow;
		}
	}
	uint32_t size;
	if (!FJS_Util_Mul32u(length, elementSize, &size)) {
		return FJS_Error_SizeOverflow;
	}

	struct NDArray* array = FJS_NDArray_Create(dimensions, length, shape, datatype);
	if (array == NULL) {
		return FJS_Error_OutOfMemory;
	}

	memset(FJS_NDArray_GetData(array), 0, size);

	FJS_AllocateId(instance, idOut, array);
	return FJS_Error_Ok;
}

enum FJS_Error FJS_Execute_CreateFromBuffer(PP_Instance instance, int32_t idOut, size_t dimensions, uint32_t shape[static dimensions], enum FJS_DataType datatype, uint32_t bufferSize, void* buffer) {
	if (dimensions == 0) {
		return FJS_Error_EmptyShape;
	}
	const uint32_t elementSize = FJS_DataType_GetSize(datatype);
	if (elementSize == 0) {
		return FJS_Error_InvalidDataType;
	}
	uint32_t length = 1;
	for (uint32_t dimension = 0; dimension < dimensions; dimension++) {
		const uint32_t measure = shape[dimension];
		if (measure < 1) {
			return FJS_Error_DegenerateShape;
		}
		/* This multiplication can easily overflow */
		if (!FJS_Util_Mul32u(length, measure, &length)) {
			return FJS_Error_LengthOverflow;
		}
	}
	uint32_t size;
	if (!FJS_Util_Mul32u(length, elementSize, &size)) {
		return FJS_Error_SizeOverflow;
	}
	if (size != bufferSize) {
		return FJS_Error_IncompatibleBufferSize;
	}

	struct NDArray* array = FJS_NDArray_Create(dimensions, length, shape, datatype);
	if (array == NULL) {
		return FJS_Error_OutOfMemory;
	}

	memcpy(FJS_NDArray_GetData(array), buffer, bufferSize);

	FJS_AllocateId(instance, idOut, array);
	return FJS_Error_Ok;
}

enum FJS_Error FJS_Execute_LinSpace(PP_Instance instance, int32_t idOut, double start, double stop, int32_t samples, bool closed, enum FJS_DataType dataType) {
	/* Check that the number of samples is sane */
	if (samples <= 0) {
		return FJS_Error_InvalidLength;
	}
	if (closed && (samples == 1)) {
		return FJS_Error_InvalidLength;
	}

	/* Check that the data type is supported and choose the initialization function for this data type */
	StepInitFunction initFunction;
	switch (dataType) {
		case FJS_DataType_F64:
		case FJS_DataType_F32:
			initFunction = stepInitFunctions[dataType];
			break;
		case FJS_DataType_Invalid:
		default:
			return FJS_Error_InvalidDataType;
	}

	/* Define parameters for the output array */
	const uint32_t length = samples;
	const uint32_t shape[1] = { samples };
	const uint32_t dimensions = 1;

	/* Create output array */
	struct NDArray* array = FJS_NDArray_Create(dimensions, length, shape, dataType);
	if (array == NULL) {
		return FJS_Error_OutOfMemory;
	}

	/* Associate the output array with its id */
	FJS_AllocateId(instance, idOut, array);

	/* Do the initialization */
	void* data = FJS_NDArray_GetData(array);
	const double range = stop - start;
	const double step = range / ((closed) ? samples - 1 : samples);
	initFunction(samples, start, step, data);

	return FJS_Error_Ok;
}

static void initLinearF32(int32_t samples, double start, double step, float dataOut[restrict static samples]) {
	const float startF32 = start;
	const float stepF32 = step;
	for (int32_t i = 0; i < samples; i++) {
		*dataOut++ = startF32 + stepF32 * ((float) i);
	}
}

static void initLinearF64(int32_t samples, double start, double step, double dataOut[restrict static samples]) {
	for (int32_t i = 0; i < samples; i++) {
		*dataOut++ = start + step * ((double) i);
	}
}
