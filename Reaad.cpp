#include <array>
#include <eccodes.h>
#include <fstream>
#include <iostream>
#include <set>
#include <stdio.h>
#include <string>
#include <vector>

int main()
{
    FILE *in;
    if (fopen_s(&in, "E:\\TestDelete\\eccodes\\wind_lat_0_-90_lon_0_360.grb2", "rb") != 0)
    {
        std::cout << "Error: Can not open file." << std::endl;
        return 0;
    }

    codes_context *cc = codes_context_get_default();
    {
        codes_context_set_definitions_path(cc, "E:\\TestDelete\\eccodes\\share\\eccodes\\definitions");
        codes_context_set_samples_path(cc, "E:\\TestDelete\\eccodes\\share\\eccodes\\samples");
        codes_gribex_mode_on(cc);
        codes_grib_multi_support_on(cc);
    }

    codes_handle *h = NULL;
    int err = 0;
    try
    {
        while ((h = codes_handle_new_from_file(cc, in, ProductKind::PRODUCT_GRIB, &err)) != NULL)
        {
            long width = 0;
            long height = 0;
            {
                long validity_date = 0;
                long validity_time = 0;

                CODES_CHECK(codes_get_long(h, "validityDate", &validity_date), 0);
                CODES_CHECK(codes_get_long(h, "validityTime", &validity_time), 0);

                auto validity_date_str = std::to_string(validity_date);
                auto validity_time_str = std::to_string(validity_time);

                std::cout << "validityDate: " << validity_date_str << std::endl;
                std::cout << "validityTime: " << validity_time_str << std::endl;
            }
            {
                CODES_CHECK(codes_get_long(h, "Ni", &width), 0);
                std::cout << "width: " << width << std::endl;
            }
            {
                CODES_CHECK(codes_get_long(h, "Nj", &height), 0);
                std::cout << "height: " << height << std::endl;
            }
            {
                long data;
                CODES_CHECK(codes_get_long(h, "paramId", &data), 0);
                std::cout << "type: " << data << std::endl;
            }
            {
                size_t str_length = 0;
                CODES_CHECK(codes_get_length(h, "parameterUnits", &str_length), 0);
                str_length = str_length * sizeof(char);
                char *key_value = (char *)malloc(str_length);
                CODES_CHECK(codes_get_string(h, "parameterUnits", key_value, &str_length), 0);
                std::cout << "units: " << std::string(key_value) << std::endl;
                free(key_value);
            }
            {
                size_t str_length = 0;
                CODES_CHECK(codes_get_length(h, "parameterName", &str_length), 0);
                str_length = str_length * sizeof(char);
                char *key_value = (char *)malloc(str_length);
                CODES_CHECK(codes_get_string(h, "parameterName", key_value, &str_length), 0);
                std::cout << "name: " << std::string(key_value) << std::endl;
                free(key_value);
            }
            {
                size_t str_length = 0;
                CODES_CHECK(codes_get_length(h, "name", &str_length), 0);
                str_length = str_length * sizeof(char);
                char *key_value = (char *)malloc(str_length);
                CODES_CHECK(codes_get_string(h, "name", key_value, &str_length), 0);
                std::cout << "description: " << std::string(key_value) << std::endl;
                free(key_value);
            }

            {
                double data;
                CODES_CHECK(codes_get_double(h, "maximum", &data), 0);
                std::cout << "max: " << data << std::endl;
            }
            {
                double data;
                CODES_CHECK(codes_get_double(h, "minimum", &data), 0);
                std::cout << "min: " << data << std::endl;
            }
            {
                double data;
                CODES_CHECK(codes_get_double(h, "average", &data), 0);
                std::cout << "average: " << data << std::endl;
            }

            {
                codes_iterator *iter = codes_grib_iterator_new(h, 0, &err);
                if (err != CODES_SUCCESS)
                    CODES_CHECK(err, 0);

                double lat, lon, value;

                int n = 0;
                while (codes_grib_iterator_next(iter, &lat, &lon, &value))
                {
                    size_t row = n / width;
                    size_t column = n % width;

                    n++;
                }

                codes_grib_iterator_delete(iter);
            }
            codes_handle_delete(h);
        }
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

    fclose(in);
    return 0;
}