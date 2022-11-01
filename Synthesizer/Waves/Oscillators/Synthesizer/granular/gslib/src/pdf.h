typedef enum distribution_enum
{ D_UNIFORM,
  D_UNIFORM_FORWARD,
  D_UNIFORM_BACKWARD,
  D_GAUSSIAN,
  D_GAUSSIAN_FORWARD,
  D_GAUSSIAN_BACKWARD,
  D_SPIKE
}
distribution;

typedef struct distribution_struct
{
  distribution *types;
  double *widths;
  double *centres;
  int n_mixtures;
  int max_mixtures;
} pdf;

pdf *create_pdf(int max_mixtures);
void set_pdf_single(pdf *pdf_data, distribution type, double width, double centre);
void set_pdf(pdf *pdf_data, int n_mixtures, distribution *types, double *widths, double *centres);
double draw_sample(pdf *pdf_data);
