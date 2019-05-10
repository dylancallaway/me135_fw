#include <iostream>

int *trajectory(int past_point[2], int cur_point[2], int bot_left[2], int top_right[2])
{
    int x0 = past_point[0], y0 = past_point[1];
    int x1 = cur_point[0], y1 = cur_point[1];
    int delta_x = (x1 - x0) / 8, delta_y = (y1 - y0) / 8;

    int *all_points = new int[1600]; //8 steps/frame * 100 frames * 2 points/step
    all_points[0] = x1;              //0-799 x
    all_points[800] = y1;            //800-1599 y

    for (int i = 1; i < 800; i++)
    {
        int x_next = all_points[i - 1] + delta_x; // I think this is supposed to be: x_next = all_points[i-1] + i*delta_x
        int y_next = all_points[i + 799] + delta_y;

        if (x_next < bot_left[0])
        {
            x_next = (37 * bot_left[0] - 17 * x_next) / 20; //(1+k)min - kx
            delta_x = -17 * delta_x / 20;
        }
        else if (x_next > top_right[0])
        {
            x_next = (37 * top_right[0] - 17 * x_next) / 20; //(1+k)max-kx
            delta_x = -17 * delta_x / 20;
        }
        if (y_next < bot_left[1])
        {
            y_next = (37 * bot_left[1] - 17 * y_next) / 20;
            delta_y = -17 * delta_y / 20;
        }
        else if (y_next > top_right[1])
        {
            y_next = (37 * top_right[1] - 17 * y_next) / 20;
            delta_y = -17 * delta_y / 20;
        }
        all_points[i] = x_next;
        all_points[i + 799] = y_next;
    }
    return all_points;
}