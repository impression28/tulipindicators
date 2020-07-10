#include "indicators.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Uso: ./a.out [periodo] [dados]\n");
		return 0;
	}
	const double period = atof(argv[1]);
	const double options[] = {period};

	const int start = ti_sma_start(options);

	const int input_size = argc - 2;

	const int output_size = input_size - start;
	if (output_size <= 0)
	{
		printf("Período muito longo em relação aos dados\n");
		return 0;
	}

	// Preparando os dados
	double *data_in = malloc(input_size*sizeof(double));
	double *data_out = malloc(output_size*sizeof(double));

	for (int i = 0; i < input_size; i++)
	{
		data_in[i] = atof(argv[i+2]);
	}

	// Cálculo normal
	const double *all_inputs[] = {data_in};
	double *all_outputs[] = {data_out};

	ti_sma(input_size, all_inputs, options, all_outputs);

	printf("Resultado normal:\n");
	for (int i = 0; i < output_size - 1; i++)
	{
		printf("%f ", data_out[i]);
		// aproveitar o loop para apagar o resultado
		// pro próximo teste
		data_out[i] = 0;
	}
	printf("%f\n\n", data_out[output_size - 1]);
	data_out[output_size - 1] = 0;

	// Cálculo com stream
	ti_stream *stream_data;
	ti_sma_stream_new(options, &stream_data);

	printf("Resultados com streaming:\n");
	int progress = 0;
	while (progress < output_size)
	{
		ti_sma_stream_run(stream_data, 1, all_inputs, all_outputs);
		progress = ti_stream_get_progress(stream_data);
		const int data_shift = (progress < 0 ? 0 : progress);
		all_inputs[0]++;
		all_outputs[0] = data_out+data_shift;

		// Print de debug
		for (int i = 0; i < output_size - 1; i++)
		{
			printf("%f ", data_out[i]);
		}
		printf("%f\n", data_out[output_size - 1]);
		printf("outputs calculated so far %d\n", progress);
	}

	ti_sma_stream_free(stream_data);

	free(data_in);
	free(data_out);
}
