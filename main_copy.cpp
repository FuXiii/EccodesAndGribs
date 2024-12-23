#include <array>
#include <eccodes.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

std::vector<std::vector<std::array<double, 4>>> ReadGribFile(const std::string &url)
{
    std::vector<std::vector<std::array<double, 4>>> result;

    {
        size_t width = 0;
        size_t height = 0;
        std::vector<std::vector<std::array<double, 3>>> u_pixels;
        std::vector<std::vector<std::array<double, 3>>> v_pixels;
        size_t index = 0;

        FILE *in;
        if (fopen_s(&in, url.c_str(), "rb") != 0)
        {
            std::cout << "Error: Can not open file." << std::endl;
            return result;
        }

        codes_context *cc = codes_context_get_default();
        {
            codes_context_set_definitions_path(cc, "E:\\EccodesAndGribs\\share\\eccodes\\definitions");
            codes_context_set_samples_path(cc, "E:\\EccodesAndGribs\\share\\eccodes\\samples");
            codes_gribex_mode_on(cc);
            codes_grib_multi_support_on(cc);
        }

        codes_handle *h = NULL;
        int err = 0;
        try
        {
            while ((h = codes_handle_new_from_file(cc, in, ProductKind::PRODUCT_GRIB, &err)) != NULL)
            {
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
                    long data;
                    CODES_CHECK(codes_get_long(h, "Ni", &data), 0);
                    std::cout << "width: " << data << std::endl;
                    width = data;
                }
                {
                    long data;
                    CODES_CHECK(codes_get_long(h, "Nj", &data), 0);
                    std::cout << "height: " << data << std::endl;
                    height = data;
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

                    std::vector<std::vector<std::array<double, 3>>> pixels;
                    {
                        pixels.resize(height);
                        for (auto &pixel_row : pixels)
                        {
                            pixel_row.resize(width);
                        }
                    }

                    int n = 0;
                    while (codes_grib_iterator_next(iter, &lat, &lon, &value))
                    {
                        size_t row = n / width;
                        size_t column = n % width;

                        auto &pixel = pixels[row][column];
                        pixel.at(0) = lat;
                        pixel.at(1) = lon;
                        pixel.at(2) = value;

                        //{
                        //    std::cout << "(";
                        //    std::cout << u_pixels[row][column].at(0) << ",";
                        //    std::cout << u_pixels[row][column].at(1) << ",";
                        //    std::cout << u_pixels[row][column].at(2);
                        //    std::cout << ")";
                        //}
                        n++;
                    }

                    if (index == 0)
                    {
                        u_pixels = pixels;
                    }
                    if (index == 1)
                    {
                        v_pixels = pixels;
                        break;
                    }

                    codes_grib_iterator_delete(iter);
                }
                codes_handle_delete(h);
                index = index + 1;
            }
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }

        {
            result.resize(height);
            for (auto &pixel_row : result)
            {
                pixel_row.resize(width);
            }
        }

        {

            for (size_t row = 0; row < height; row++)
            {
                for (size_t column = 0; column < width; column++)
                {
                    result[row][column].at(0) = u_pixels[row][column].at(0);
                    result[row][column].at(0) = u_pixels[row][column].at(1);
                    result[row][column].at(0) = u_pixels[row][column].at(2);
                    result[row][column].at(0) = v_pixels[row][column].at(2);
                }
            }
        }

        fclose(in);
    }
    std::cout << "--------------------------------" << std::endl;

    return result;
}

int main()
{
    try
    {
        {
            auto meta = ReadGribFile("E:\\EccodesAndGribs\\wind_lat_0_90_lon_0_360.grb2");
            {
                size_t height = (meta).size();
                size_t width = (meta)[0].size();

                {
                    std::string filename = "wind_lat_0_90_lon_0_360.data";
                    // std::ofstream ostrm(filename, std::ios::binary | std::ios::app | std::ios::ate);
                    std::ofstream ostrm(filename, std::ios::binary);
                    // for (size_t row = 0; row < height; row++)
                    for (size_t row = height - 1; row >= 0; row--)
                    {
                        for (size_t column = 0; column < width; column++)
                        {
                            ostrm << "(";
                            ostrm << meta[row][column].at(0) << ",";
                            ostrm << meta[row][column].at(1) << ",";
                            ostrm << meta[row][column].at(2) << ",";
                            ostrm << meta[row][column].at(3);
                            ostrm << ") ";
                        }
                        ostrm << std::endl;
                    }
                }
            }
        }

        return 0;

        {
            auto meta = ReadGribFile("E:\\EccodesAndGribs\\wind_lat_0_-90_lon_0_360.grb2");

            size_t height = (meta).size();
            size_t width = (meta)[0].size();

            {

                std::string filename = "wind_lat_0_-90_lon_0_360.data";
                std::ofstream ostrm(filename, std::ios::binary);

                // for (size_t row = 0; row < height; row++)
                for (size_t row = height - 1; row >= 0; row--)
                {
                    for (size_t column = 0; column < width; column++)
                    {
                        ostrm << "(";
                        ostrm << (meta)[row][column].at(0) << ",";
                        ostrm << (meta)[row][column].at(1) << ",";
                        ostrm << (meta)[row][column].at(2) << ",";
                        ostrm << (meta)[row][column].at(3);
                        ostrm << ") ";
                    }
                    ostrm << std::endl;
                }
            }
        }
    }
    catch (std::exception &e)
    {
        std::cout << "!!!Exception:" << e.what() << std::endl;
    }

    return 0;
}