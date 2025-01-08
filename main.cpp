#include <array>
#include <eccodes.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

bool write_binary = true;
bool is_write_float = true;

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
                    result[row][column].at(1) = u_pixels[row][column].at(1);
                    result[row][column].at(2) = u_pixels[row][column].at(2);
                    result[row][column].at(3) = v_pixels[row][column].at(2);
                }
            }
        }

        fclose(in);
    }
    std::cout << "--------------------------------" << std::endl;

    return result;
}

int Write()
{
    try
    {
        size_t all_height = 0;
        size_t all_width = 0;

        std::string all_filename = "all.data";
        std::ofstream all_ostrm(all_filename, std::ios::binary);

        {
            auto meta = ReadGribFile("E:\\EccodesAndGribs\\wind_lat_0_-90_lon_0_360.grb2");

            size_t height = (meta).size();
            size_t width = (meta)[0].size();

            all_width = width;
            all_height += height;

            {

                std::string filename = "wind_lat_0_-90_lon_0_360.data";
                std::ofstream ostrm(filename, std::ios::binary);

                if (write_binary)
                {
                    for (size_t row = 0; row < height; row++)
                    {
                        for (size_t column = 0; column < width; column++)
                        {
                            if (is_write_float)
                            {
                                float _0 = meta[row][column].at(0);
                                float _1 = meta[row][column].at(1);
                                float _2 = meta[row][column].at(2);
                                float _3 = meta[row][column].at(3);

                                ostrm.write(reinterpret_cast<char *>(&_0), sizeof _0);
                                ostrm.write(reinterpret_cast<char *>(&_1), sizeof _1);
                                ostrm.write(reinterpret_cast<char *>(&_2), sizeof _2);
                                ostrm.write(reinterpret_cast<char *>(&_3), sizeof _3);

                                all_ostrm.write(reinterpret_cast<char *>(&_0), sizeof _0);
                                all_ostrm.write(reinterpret_cast<char *>(&_1), sizeof _1);
                                all_ostrm.write(reinterpret_cast<char *>(&_2), sizeof _2);
                                all_ostrm.write(reinterpret_cast<char *>(&_3), sizeof _3);
                            }
                            else
                            {
                                ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(0)),
                                            sizeof meta[row][column].at(0));
                                ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(1)),
                                            sizeof meta[row][column].at(1));
                                ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(2)),
                                            sizeof meta[row][column].at(2));
                                ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(3)),
                                            sizeof meta[row][column].at(3));

                                all_ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(0)),
                                                sizeof meta[row][column].at(0));
                                all_ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(1)),
                                                sizeof meta[row][column].at(1));
                                all_ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(2)),
                                                sizeof meta[row][column].at(2));
                                all_ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(3)),
                                                sizeof meta[row][column].at(3));
                            }
                        }
                    }
                }
                else
                {
                    for (size_t row = 0; row < height; row++)
                    {
                        for (size_t column = 0; column < width; column++)
                        {
                            ostrm << "(";
                            ostrm << meta[row][column].at(0) << ", ";
                            ostrm << meta[row][column].at(1) << ", ";
                            ostrm << meta[row][column].at(2) << ", ";
                            ostrm << meta[row][column].at(3);
                            ostrm << ") ";

                            all_ostrm << "(";
                            all_ostrm << meta[row][column].at(0) << ", ";
                            all_ostrm << meta[row][column].at(1) << ", ";
                            all_ostrm << meta[row][column].at(2) << ", ";
                            all_ostrm << meta[row][column].at(3);
                            all_ostrm << ") ";
                        }
                        ostrm << std::endl;
                        all_ostrm << std::endl;
                    }
                }
            }
        }

        {
            auto meta = ReadGribFile("E:\\EccodesAndGribs\\wind_lat_0_90_lon_0_360.grb2");
            {
                size_t height = (meta).size();
                size_t width = (meta)[0].size();

                all_height += height - 1;

                {
                    std::string filename = "wind_lat_0_90_lon_0_360.data";
                    std::ofstream ostrm(filename, std::ios::binary);

                    if (write_binary)
                    {
                        for (size_t row = 1; row < height; row++)
                        {
                            for (size_t column = 0; column < width; column++)
                            {
                                if (is_write_float)
                                {
                                    float _0 = meta[row][column].at(0);
                                    float _1 = meta[row][column].at(1);
                                    float _2 = meta[row][column].at(2);
                                    float _3 = meta[row][column].at(3);

                                    ostrm.write(reinterpret_cast<char *>(&_0), sizeof _0);
                                    ostrm.write(reinterpret_cast<char *>(&_1), sizeof _1);
                                    ostrm.write(reinterpret_cast<char *>(&_2), sizeof _2);
                                    ostrm.write(reinterpret_cast<char *>(&_3), sizeof _3);

                                    all_ostrm.write(reinterpret_cast<char *>(&_0), sizeof _0);
                                    all_ostrm.write(reinterpret_cast<char *>(&_1), sizeof _1);
                                    all_ostrm.write(reinterpret_cast<char *>(&_2), sizeof _2);
                                    all_ostrm.write(reinterpret_cast<char *>(&_3), sizeof _3);
                                }
                                else
                                {
                                    ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(0)),
                                                sizeof meta[row][column].at(0));
                                    ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(1)),
                                                sizeof meta[row][column].at(1));
                                    ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(2)),
                                                sizeof meta[row][column].at(2));
                                    ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(3)),
                                                sizeof meta[row][column].at(3));

                                    all_ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(0)),
                                                    sizeof meta[row][column].at(0));
                                    all_ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(1)),
                                                    sizeof meta[row][column].at(1));
                                    all_ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(2)),
                                                    sizeof meta[row][column].at(2));
                                    all_ostrm.write(reinterpret_cast<char *>(&meta[row][column].at(3)),
                                                    sizeof meta[row][column].at(3));
                                }
                            }
                        }
                    }
                    else
                    {
                        for (size_t row = 1; row < height; row++)
                        {
                            for (size_t column = 0; column < width; column++)
                            {
                                ostrm << "(";
                                ostrm << meta[row][column].at(0) << ", ";
                                ostrm << meta[row][column].at(1) << ", ";
                                ostrm << meta[row][column].at(2) << ", ";
                                ostrm << meta[row][column].at(3);
                                ostrm << ") ";

                                all_ostrm << "(";
                                all_ostrm << meta[row][column].at(0) << ", ";
                                all_ostrm << meta[row][column].at(1) << ", ";
                                all_ostrm << meta[row][column].at(2) << ", ";
                                all_ostrm << meta[row][column].at(3);
                                all_ostrm << ") ";
                            }
                            ostrm << std::endl;
                            all_ostrm << std::endl;
                        }
                    }
                }
            }
        }

        if (write_binary)
        {
            all_ostrm.write(reinterpret_cast<char *>(&all_width), sizeof all_width);
            all_ostrm.write(reinterpret_cast<char *>(&all_height), sizeof all_height);
        }
        else
        {
            all_ostrm << "(";
            all_ostrm << all_width << ", ";
            all_ostrm << all_height;
            all_ostrm << ") ";
        }

        {
            std::cout << "all_size: ";
            std::cout << "(";
            std::cout << all_width << ", ";
            std::cout << all_height;
            std::cout << ") ";
        }
    }
    catch (std::exception &e)
    {
        std::cout << "!!!Exception:" << e.what() << std::endl;
    }

    return 0;
}

#include "GlobalWind.h"
void Read()
{
    if (false)
    {
        std::ifstream istrm("E:\\EccodesAndGribs\\all.bin", std::ios::binary | std::ios::in | std::ios::ate);
        {
            size_t width = 0;
            size_t height = 0;
            {
                size_t end = istrm.tellg();
                istrm.seekg(end - sizeof(size_t) * 2);

                istrm.read(reinterpret_cast<char *>(&width), sizeof(size_t));
                istrm.read(reinterpret_cast<char *>(&height), sizeof(size_t));

                std::cout << "width: " << width << std::endl;
                std::cout << "height: " << height << std::endl;
            }

            auto GetOffset = [&](size_t row, size_t column) { return (row * width + column) * sizeof(double) * 4; };

            std::string filename = "compare.data";
            std::ofstream ostrm(filename, std::ios::binary);
            {
                size_t index = 0;
                for (size_t row = 0; row < height; row++)
                {
                    for (size_t column = 0; column < width; column++)
                    {
                        istrm.seekg(GetOffset(row, column));

                        double lat = 0;
                        double lon = 0;
                        double u = 0;
                        double v = 0;

                        istrm.read(reinterpret_cast<char *>(&lat), sizeof(lat));
                        istrm.read(reinterpret_cast<char *>(&lon), sizeof(lon));
                        istrm.read(reinterpret_cast<char *>(&u), sizeof(u));
                        istrm.read(reinterpret_cast<char *>(&v), sizeof(v));
                        // std::cout << "lat: " << lat << std::endl;
                        // std::cout << "lon: " << lon << std::endl;
                        // std::cout << "u: " << u << std::endl;
                        // std::cout << "v: " << v << std::endl;
                        // std::cout << "------" << v << std::endl;
                        ostrm << "(";
                        ostrm << lat << ", ";
                        ostrm << lon << ", ";
                        ostrm << u << ", ";
                        ostrm << v;
                        ostrm << ") ";
                    }
                    ostrm << std::endl;
                }
            }
        }
    }
    else
    {
        GlobalWind gw("E:\\EccodesAndGribs\\all.bin");
        auto width = gw.Width();
        auto height = gw.Height();

        std::cout << "width: " << width << std::endl;
        std::cout << "height: " << height << std::endl;

        for (size_t row = 0; row < height; row++)
        {
            for (size_t column = 0; column < width; column++)
            {
                auto value = gw.Get(row, column);
                std::cout << "(";
                std::cout << value.lat << ", ";
                std::cout << value.lon << ", ";
                std::cout << value.u << ", ";
                std::cout << value.v;
                std::cout << ") ";
                std::cout << std::endl;
            }
        }
    }
}

int main()
{
    // Read();
    Write();
    return 0;
}

template <typename T> void ReverseTraversal(const std::vector<T> &vec)
{
    if (!vec.empty())
    {
        for (size_t index = vec.size() - 1; index >= 0; index--)
        {
            auto &item = vec[index];
        }
    }
}
