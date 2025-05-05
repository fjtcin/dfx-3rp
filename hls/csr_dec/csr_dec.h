#pragma once

struct pack {
	float num0;
	float num1;
	float num2;
	float num3;
};

void csr_dec(hls::stream<pack>& in, hls::stream<pack>& out);
