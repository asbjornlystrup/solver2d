// SPDX-FileCopyrightText: 2024 Erin Catto
// SPDX-License-Identifier: MIT

#define _CRT_SECURE_NO_WARNINGS
#include "settings.h"

#include <jsmn.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* fileName = "settings.ini";

// Load a file. You must free the character array.
static bool ReadFile(char*& data, int& size, const char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (file == nullptr)
	{
		return false;
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (size == 0)
	{
		return false;
	}

	data = (char*)malloc(size + 1);
	fread(data, size, 1, file);
	fclose(file);
	data[size] = 0;

	return true;
}

void Settings::Save()
{
	FILE* file = fopen(fileName, "w");
	fprintf(file, "{\n");
	fprintf(file, "  \"sampleIndex\": %d,\n", sampleIndex);
	fprintf(file, "  \"drawShapes\": %s,\n", drawShapes ? "true" : "false");
	fprintf(file, "  \"drawJoints\": %s,\n", drawJoints ? "true" : "false");
	fprintf(file, "  \"drawAABBs\": %s,\n", drawAABBs ? "true" : "false");
	fprintf(file, "  \"drawContactPoints\": %s,\n", drawContactPoints ? "true" : "false");
	fprintf(file, "  \"drawContactNormals\": %s,\n", drawContactNormals ? "true" : "false");
	fprintf(file, "  \"drawContactImpulse\": %s,\n", drawContactImpulse ? "true" : "false");
	fprintf(file, "  \"drawFrictionImpulse\": %s,\n", drawFrictionImpulse ? "true" : "false");
	fprintf(file, "  \"drawMass\": %s,\n", drawMass ? "true" : "false");
	fprintf(file, "  \"drawStats\": %s,\n", drawStats ? "true" : "false");
	fprintf(file, "  \"enableWarmStarting\": %s,\n", enableWarmStarting ? "true" : "false");
	fprintf(file, "}\n");
	fclose(file);
}

static int jsoneq(const char* json, jsmntok_t* tok, const char* s)
{
	if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start && strncmp(json + tok->start, s, tok->end - tok->start) == 0)
	{
		return 0;
	}
	return -1;
}

#define MAX_TOKENS 32

void Settings::Load()
{
	char* data = nullptr;
	int size = 0;
	bool found = ReadFile(data, size, fileName);
	if (found ==  false)
	{
		return;
	}

	jsmn_parser parser;
	jsmntok_t tokens[MAX_TOKENS];

	jsmn_init(&parser);

	// js - pointer to JSON string
	// tokens - an array of tokens available
	// 10 - number of tokens available
	int tokenCount = jsmn_parse(&parser, data, size, tokens, MAX_TOKENS);
	char buffer[32];

	for (int i = 0; i < tokenCount; ++i)
	{
		if (jsoneq(data, &tokens[i], "sampleIndex") == 0)
		{
			int count = tokens[i + 1].end - tokens[i + 1].start;
			assert(count < 32);
			const char* s = data + tokens[i + 1].start;
			strncpy(buffer, s, count);
			char* dummy;
			sampleIndex = strtol(buffer, &dummy, 10);
		}
		else if (jsoneq(data, &tokens[i], "drawShapes") == 0)
		{
			const char* s = data + tokens[i + 1].start;
			if (strncmp(s, "true", 4) == 0)
			{
				drawShapes = true;
			}
			else if (strncmp(s, "false", 5) == 0)
			{
				drawShapes = false;
			}
		}
	}

	free(data);

	enabledSolvers[s2_solverTGS_Soft_XPBD] = true;
}
