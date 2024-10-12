#include <ros/ros.h> //ros依赖
// #include <image_transport/image_transport.h>//目前不需要
#include <cv_bridge/cv_bridge.h>         //需要用这个转换ros->opencv
#include <sensor_msgs/image_encodings.h> //获取ros图像
// #include <opencv2/imgproc/imgproc.hpp>//目前不需要
#include <opencv2/highgui/highgui.hpp> //图像交互界面，显示图像
#include <string>

static const std::string RGB_WINDOW = "RGB Image";
static const std::string DEPTH_WINDOW = "Depth Image";

void ImageConverte_and_Show(const sensor_msgs::ImageConstPtr &msg, const std::string &title, bool is_depth)
{
    cv_bridge::CvImagePtr cv_image_ptr;
    try {
        if (is_depth) {
            // 深度图像保持原始格式（16UC1），不转换为BGR8
            cv_image_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_16UC1);

            // 归一化深度图像以便显示
            cv::Mat depth_img_normalized;
            cv::normalize(cv_image_ptr->image, depth_img_normalized, 0, 255, cv::NORM_MINMAX);
            depth_img_normalized.convertTo(depth_img_normalized, CV_8UC1); // 转换为8位图像

            // 显示深度图像
            cv::imshow(title, depth_img_normalized);
        } else {
            // 彩色图像转换为BGR8格式
            cv_image_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
            // 显示彩色图像
            cv::imshow(title, cv_image_ptr->image);
        }
    } catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    if (cv::waitKey(1) == 27) {
        ros::shutdown();
        cv::destroyAllWindows();
    }
}

void color_show(const sensor_msgs::ImageConstPtr &msg)
{
    ImageConverte_and_Show(msg, RGB_WINDOW, false); // 彩色图像
}

void depth_show(const sensor_msgs::ImageConstPtr &msg)
{
    ImageConverte_and_Show(msg, DEPTH_WINDOW, true); // 深度图像
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "show_rgb_and_depth_graph");
    ros::NodeHandle n;

    // 创建要显示的两个窗口
    cv::namedWindow(RGB_WINDOW);
    cv::namedWindow(DEPTH_WINDOW);

    ros::Subscriber camera_color_sub = n.subscribe("/camera/color/image_raw", 10, color_show);
    ros::Subscriber camera_depth_sub = n.subscribe("/camera/depth/image_rect_raw", 10, depth_show);

    ros::spin();
    return 0;
}
