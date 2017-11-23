#include "utils.h"
#include <math.h>

#define DEG_TO_RAD 0.0174532925199

void getModelviewMatrix(float* vals, float x, float y, float w, float h) {
	vals[0] = w;
	vals[1] = 0;
	vals[2] = 0;
	vals[3] = 0;

	vals[4] = 0;
	vals[5] = h;
	vals[6] = 0;
	vals[7] = 0;

	vals[8] = 0;
	vals[9] = 0;
	vals[10] = 1;
	vals[11] = 0;

	vals[12] = x;
	vals[13] = y;
	vals[14] = 0;
	vals[15] = 1;
}

void getModelviewMatrixR(float* vals, float x, float y, float w, float h, float r) {
	float c = cos(r*DEG_TO_RAD);
	float s = sin(r*DEG_TO_RAD);

	vals[0] = w*c;
	vals[1] = w*s;
	vals[2] = 0;
	vals[3] = 0;

	vals[4] = -h*s;
	vals[5] = h*c;
	vals[6] = 0;
	vals[7] = 0;

	vals[8] = 0;
	vals[9] = 0;
	vals[10] = 1;
	vals[11] = 0;

	vals[12] = x;
	vals[13] = y;
	vals[14] = 0;
	vals[15] = 1;
}

void getOrthoMatrix(float* vals, float left, float right, float bottom, float top, float n, float f) {
	vals[0] = 2.0f / (right - left);
	vals[1] = 0;
	vals[2] = 0;
	vals[3] = 0;

	vals[4] = 0;
	vals[5] = 2.0f / (top - bottom);
	vals[6] = 0;
	vals[7] = 0;

	vals[8] = 0;
	vals[9] = 0;
	vals[10] = -2.0f / (f - n);
	vals[11] = 0;

	vals[12] = -(right + left) / (right - left);
	vals[13] = -(top + bottom) / (top - bottom);
	vals[14] = -(f + n) / (f - n);
	vals[15] = 1;
}

double abs_c(double n1) {
	return n1 > 0 ? n1 : -(n1);
}

void createEntry(UIState* uiState, int ID) {
	Entry* entry = malloc(sizeof(Entry));
	entry->textField = swCreateTextField(uiState->rootView, swMakeRect(10, 580 - uiState->y * 40, 285, 30), "");
	entry->submitButton = swCreateButton(uiState->rootView, swMakeRect(305, 580 - uiState->y * 40, 75, 30), "Submit", &submitCallback, entry);
	entry->deleteButton = NULL;
	entry->ID = ID;
	entry->next = NULL;

	if (uiState->first) {
		uiState->root = entry;
		uiState->last = entry;
		uiState->first = 0;
	}
	else {
		uiState->last->next = entry;
		uiState->last = entry;
	}

	uiState->y++;
}

void drawAxes(float* modelMat, GLuint modelLoc, int width, int height) {
	double l = abs_c(g_left);
	double r = abs_c(g_right);

	double xoff = (l / (r + l))* width;

	if (g_left < 0 && g_right > 0) {
		getModelviewMatrix(modelMat, xoff, 300, 1, 600);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	double d = abs_c(g_down);
	double u = abs_c(g_up);

	double yoff = (d / (u + d))* height;

	if (g_up > 0 && g_down < 0) {
		getModelviewMatrix(modelMat, 300, yoff, 600, 1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void drawGrid(float* modelMat, GLuint modelLoc, double xl, double xr, double yd, double yu, int width, int height) {
	double xratio = width / (xr - xl);

	double xlen = xr - xl;
	double xlog = log10(xlen);
	double xfloor = floor(xlog);
	double xmag = pow(10, xfloor);

	if (floor(xlog - 0.25) < xfloor) {
		xmag /= 2;
	}

	if (floor(xlog - 0.5) < xfloor) {
		xmag /= 2;
	}

	double xlr = round(xl / xmag)*xmag;
	double xrr = round(xr / xmag)*xmag;

	double xNum = (xrr - xlr) / xmag;
	for (int i = 0; i < xNum + 1;i++) {
		double tx = (xlr - xl + xmag*i)*xratio;

		if (tx  < 0 || tx > width) {
			continue;
		}

		getModelviewMatrix(modelMat, tx, 300, 0.25f, 600);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	double yratio = height / (yu - yd);

	double ylen = yu - yd;
	double ylog = log10(ylen);
	double yfloor = floor(ylog);
	double ymag = pow(10, yfloor);

	if (floor(ylog - 0.25) < yfloor) {
		ymag /= 2;
	}

	if (floor(ylog - 0.5) < yfloor) {
		ymag /= 2;
	}

	double ylr = round(yd / ymag)*ymag;
	double yrr = round(yu / ymag)*ymag;

	double yNum = (yrr - ylr) / ymag;

	for (int i = 0; i < yNum * 2; i++) {
		double ty = (ylr - yd + ymag*i)*yratio;

		if (ty < 0 || ty > height) {
			continue;
		}
		getModelviewMatrix(modelMat, 300, ty, 600, 0.25f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void mouseUpdate(SMouseState* state, SMouseState* preState, SRect* bounds) {
	float posx = state->x;
	float posy = state->y;

	uint8_t down = state->ldown;

	uint8_t hovering = posx >= bounds->x &&
		posx <= bounds->x + bounds->width &&
		620 - posy >= bounds->y &&
		620 - posy <= bounds->y + bounds->height;

	posx -= bounds->x;
	posy -= (620 - (bounds->y + bounds->height));

	if (hovering) {
		if (preState->x != posx || preState->y != posy || preState->ldown != down) {

			float dx = posx - preState->x;
			float dy = posy - preState->y;

			preState->x = posx;
			preState->y = posy;
			preState->ldown = down;

			if ((dx != 0 || dy != 0) && down) {
				double width = g_right - g_left;
				double percentX = dx / g_windowWidth;

				double moveX = -width * percentX;

				g_left += moveX;
				g_right += moveX;

				double height = g_up - g_down;
				double percentY = dy / g_windowHeight;

				double moveY = height * percentY;

				g_down += moveY;
				g_up += moveY;
			}
		}
	}

}
