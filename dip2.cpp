#include <bits/stdc++.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>


using namespace std;
using namespace cv;



void delaunay_Triangulation( Mat& img, Subdiv2D& subdiv,vector<Vec6f>& triangleList );
void read_Points_From_File(vector<Point2f>& points,char* pointsFileName);
void morphing(Mat &img1, Mat &img2, Mat &img, vector<Point2f> &p1, vector<Point2f> &p2, vector<Point2f> &p, double alpha);
int Bilinear_Interpolation(Mat img1, double x,double y);
void Pushing_into_rectangles(vector<Point2f> &t1, vector<Point2f> &t2, vector<Point2f> &t,vector<Point2f>& t1Rect,vector<Point2f>& t2Rect,vector<Point2f>& tRect,vector<Point>& tRectInt);


 

int main ()
{
   // uchar arr[3][3] = {100,100,100,100,20,30,40,40,30};
   // Mat img(3,3, CV_8UC1, arr);
   // uchar arr2[3][3] = {100,100,100,100,20,30,40,40,30};
   // Mat img2(3,3, CV_8UC1, arr2);
    
     string str1="4.jpg",str2="5.jpg";
  //  cout <<"Enter the initial image name:"<<endl;
    //cin >> str1;
   // cout <<"Enter the final image name:"<<endl;
  //  cin >> str2;
    Mat img = imread(str1,1);
    
    Mat img2 = imread(str2,1);
    
    
    vector<Vec6f> triangleList;
    vector<Vec6f> triangleList_final;
    Size size = img.size();
   // cout << size.width <<" "<<size.height <<endl;
    Rect rect(0,0, size.width, size.height);      
    Subdiv2D subdiv(rect);
    vector<Point2f> points;
    vector<Point2f> points2;
    vector<Point2f> points_new;
    char str3[1000];
    char str4[1000];
     //string str3="points1.txt",str4="points3.txt";
    cout <<"Enter the initial image name:"<<endl;
    cin >> str3;
   cout <<"Enter the final image name:"<<endl;
    cin >> str4;
    read_Points_From_File(points,str3);
   // cout << points.size() <<endl;
    read_Points_From_File(points2,str4);
  //  cout << points2.size() <<endl;
    ofstream myfile;
    
  
    


   // Insert points into subdiv
    for( vector<Point2f>::iterator it = points.begin(); it != points.end(); it++)
    {
            subdiv.insert(*it);            
            
    }
    delaunay_Triangulation(img,subdiv,triangleList);

    
    
    for( size_t i = 0; i < triangleList.size(); i++ )
    {
        Vec6f t = triangleList[i];
        if(t[0] < size.width && t[0]>=0 && t[1]>=0 && t[2]>=0 && t[3]>=0 && t[4]>=0 && t[5]>=0 && t[1] < size.height && t[2] < size.width && t[3] < size.height && t[4] < size.width && t[5] < size.height)
  
        triangleList_final.push_back(t);
               
    }
    
    
    
    
      myfile.open ("example.txt");
     //cout << triangleList_final.size() <<endl;
     for( size_t i = 0; i < triangleList_final.size(); i++ )
    {
         Vec6f t = triangleList_final[i];
         for( size_t j = 0; j < points.size(); j++)
       {
         if(t[0] == points[j].x && t[1] == points[j].y)
          myfile << j << " ";
         if(t[2] == points[j].x && t[3] == points[j].y)
          myfile << j << " ";
         if(t[4] == points[j].x && t[5] == points[j].y)
          myfile << j << " ";  
       }   
         myfile << "\n";
      // cout << t[0] << " "<<t[1]<<" " <<t[2]<< " "<<t[3]<<" " <<t[4]<< " "<<t[5] <<endl;
               
    }
     myfile.close();
     
    
     int number;
     cout << "Enter the number of intermediate images :"  << endl;
     cin >> number;
     
     double alpha = double (1/(double)(1+number)); 
    // cout << alpha << endl;
     double beta = 0;
     
     for(int k=0;k<=number;k++)
   { 
    
     beta = k * alpha;
     for(int i = 0; i < points.size(); i++)
    {
        float x, y;
        x = (1 - beta) * points[i].x + beta * points2[i].x;
        y =  ( 1 - beta ) * points[i].y + beta * points2[i].y;  
        points_new.push_back(Point2f(x,y));
        
    }
    
     // for adjusting to any size :
    
    
    int rows_difference = img2.rows - img.rows;
    int cols_difference = img2.cols - img.cols;
    int new_rows = round(img.rows + (beta * rows_difference));
    int new_cols = round(img.cols + (beta * cols_difference));
    
    
    
    //empty average image
    Mat imgMorph = Mat::zeros(new_rows,new_cols, CV_32FC3);
    
    img.convertTo(img, CV_32F);
    img2.convertTo(img2, CV_32F); 
    
    
    ifstream ptr("example.txt");
    int x,y,z;
    
    while(ptr >> x >> y >> z)
    {
        vector<Point2f> t1, t2, t;
        t1.push_back( points[x] );
        t1.push_back( points[y] );
        t1.push_back( points[z] );
        t.push_back( points_new[x] );
        t.push_back( points_new[y] );
        t.push_back( points_new[z] );
        t2.push_back( points2[x] );
        t2.push_back( points2[y] );
        t2.push_back( points2[z] );
        
        morphing(img, img2, imgMorph, t1, t2, t, beta);
        
     }   
     ptr.close();
     
     
     
     
     
     
     /*
     
     int num;
     cout << "Enter 1 for Morphing in Affine Transformations OR Enter 2 for Generic Morphing for two images :"<<endl;
     cin >> num ;
     if(num == 1)
     {
          int p;
          cout << "Enter 1 for Rotated output :"<<endl;
          cin >> p;
          if (p == 1)
          {
          cout << "Enter angle in degrees :"<<endl;
          double angle;
          cin >> angle;  
          float array[2][3];
          cout << "Enter 2x3 Transformation matrix elements :"<<endl;
          for(int i=0;i<2;i++)
           {
             for(int j=0;j<3;j++)
                cin >> array[i][j];
             
           }   
          Mat warp_mat(2,3, CV_32FC1, array);
          
          Mat img = imread("4.jpg",0);
          Mat rot_mat( 2, 3, CV_32FC1 );
          
          Mat src, warp_dst, warp_rotate_dst;
          warp_dst = Mat::zeros( img.rows, img.cols, img.type() );
          
          /// Apply the Affine Transform just found to the src image
          warpAffine( img, warp_dst, warp_mat, warp_dst.size() );
          Point center = Point( warp_dst.cols/2, warp_dst.rows/2 );
          rot_mat = getRotationMatrix2D( center, angle,1);
          cout << rot_mat <<endl;
          warpAffine( warp_dst, warp_rotate_dst, rot_mat, warp_dst.size() );
          namedWindow( "MORPHED IMAGE",WINDOW_AUTOSIZE);
          imshow( "MORPHED IMAGE",warp_dst );
          waitKey(0);
          
          return 0;
                    
          // cloning :
        //  Mat img2 = warp_rotate_dst.clone();
          
          }
          else
          {
          /*
          Mat img = imread("4.jpg",1);
          warp_dst = Mat::zeros( img.rows, src.cols, src.type() );
          
          float array[2][3];
          cout << "Enter 2x3 Transformation matrix elements :"<<endl;
          for(int i=0;i<2;i++)
           {
             for(int j=0;j<3;j++)
                cin >> array[i][j];
             
           }   */
           
         //  Mat warp_mat(2,3, CV_32FC1, array);
          

     
     
     
         // }
     
  //   }   
     /************To be moved at proper place later ***************/
     string ak="abc";    
     char c ='a';
     c=c+k;
     ak.push_back(c);
   imwrite(ak.append("log2.jpg"),imgMorph);    
  namedWindow( "MORPHED IMAGE",WINDOW_AUTOSIZE);
  imshow( "MORPHED IMAGE", imgMorph/255.0);
  waitKey(1000);
  destroyAllWindows();
  //ReleaseImage(&imgMorph);

  }
  namedWindow( "MORPHED IMAGE",WINDOW_AUTOSIZE);
  imshow( "MORPHED IMAGE", img2/255.0);
  waitKey(1000);



return 0;
}



void delaunay_Triangulation( Mat& img, Subdiv2D& subdiv,vector<Vec6f>& triangleList )
{
    Size size = img.size();
    Rect rect(0,0, size.width, size.height);     
    subdiv.getTriangleList(triangleList); 
    
}

void read_Points_From_File(vector<Point2f>& points, char* str)
{
    float s, t;
    ifstream ptr(str); 
    while(ptr >> s >> t)
    {
        points.push_back(Point2f(s,t));
    }
    ptr.close();
    
}


void morphing(Mat &img1, Mat &img2, Mat &img, vector<Point2f> &p1, vector<Point2f> &p2, vector<Point2f> &p, double alpha)
{
    Mat img1Rectangle, img2Rectangle,imgRectangle,warpMat1,warpMat2;
    vector<Point2f> p1Rect, p2Rect, pRect;
    vector<Point> pRectInt;
    Rect rectangle = boundingRect(p);
    Mat warpImage1 = Mat::zeros(rectangle.height, rectangle.width, img1Rectangle.type());
    Mat warpImage2 = Mat::zeros(rectangle.height, rectangle.width, img2Rectangle.type());
    
    
    Rect rectangle1 = boundingRect(p1);
    Rect rectangle2 = boundingRect(p2);
    img1(rectangle1).copyTo(img1Rectangle);
    img2(rectangle2).copyTo(img2Rectangle);
    
    Pushing_into_rectangles(p1,p2,p,p1Rect,p2Rect,pRect,pRectInt);
   
    Mat mask = Mat::zeros(rectangle.height, rectangle.width, CV_32FC3);
    fillConvexPoly(mask, pRectInt, Scalar(1.0, 1.0, 1.0), 16, 0);
    
    warpMat1 = getAffineTransform (p1Rect,pRect);
    warpMat2 = getAffineTransform (p2Rect,pRect);
    
    warpAffine( img1Rectangle, warpImage1, warpMat1, warpImage1.size(), INTER_LINEAR, BORDER_REFLECT_101);
    warpAffine( img2Rectangle, warpImage2, warpMat2, warpImage2.size(), INTER_LINEAR, BORDER_REFLECT_101);
    
    imgRectangle = (1.0 - alpha) * warpImage1;  
    imgRectangle = imgRectangle + alpha * warpImage2;
    
    multiply(imgRectangle,mask, imgRectangle);
    multiply(img(rectangle), Scalar(1.0,1.0,1.0) - mask, img(rectangle));
    img(rectangle) = img(rectangle) + imgRectangle;
    
}


void Pushing_into_rectangles(vector<Point2f> &t1, vector<Point2f> &t2, vector<Point2f> &t,vector<Point2f>& t1Rect,vector<Point2f>& t2Rect,vector<Point2f>& tRect,vector<Point>& tRectInt)
{

    Rect rectangle = boundingRect(t);
    Rect rectangle1 = boundingRect(t1);
    Rect rectangle2 = boundingRect(t2);
    int i=0;
    while(i<3)
    {
        float m = t[i].x;
        float n = t[i].y;
        float rectx = rectangle.x;
        float recty = rectangle.y;
        float subx = m - rectx;
        float suby = n - recty;
        tRect.push_back( Point2f(subx,suby));
        tRectInt.push_back( Point2f(subx,suby));
        float m1 = t1[i].x;
        float n1 = t1[i].y;
        float rect1x = rectangle1.x;
        float rect1y = rectangle1.y;
        float sub1x = m1 - rect1x;
        float sub1y = n1 - rect1y;
        t1Rect.push_back( Point2f(sub1x,sub1y));
        float m2 = t2[i].x;
        float n2 = t2[i].y;
        float rect2x = rectangle2.x;
        float rect2y = rectangle2.y;
        float sub2x = m2 - rect2x;
        float sub2y = n2 - rect2y;
        t2Rect.push_back( Point2f(sub2x,sub2y));
        i++;
   
    }


}








int Bilinear_Interpolation(Mat img1, double x,double y)
{
//Mat img1 = imread(str,0);
int r2=img1.rows;
int c2=img1.cols;
double before_x = round(x)-0.5;
double before_y = round(y)-0.5;
double after_x =  round(x)+0.5;
double after_y =  round(y)+0.5;
double alpha = x-before_x;
double beta = y-before_y;
double ans;
int value;

//cout << before_x <<" " << x <<" " << before_y <<" "<< y << endl;

int val1,val2,val3,val4;
if(before_x > 0 && before_x < r2 && before_y > 0 && before_y < c2)
val1 = img1.at<uchar>(floor(before_x),floor(before_y));
else
val1 = 0;

if(before_x > 0 && before_x < r2 && after_y > 0 && after_y < c2)
val2 = img1.at<uchar>(floor(before_x),floor(after_y));
else
val2 = 0;

if(after_x > 0 && after_x < r2 && before_y > 0 && before_y < c2)
val3 = img1.at<uchar>(floor(after_x),floor(before_y));
else
val3 = 0;

if(after_x > 0 && after_x < r2 && after_y > 0 && after_y < c2)
val4 = img1.at<uchar>(floor(after_x),floor(after_y));
else
val4 = 0;

if(val1 != 0 && val2 == 0 && val3 == 0 && val4 == 0)
ans = val1;
else if(val1 == 0 && val2 != 0 && val3 == 0 && val4 == 0)
ans = val2;
else if(val1 == 0 && val2 == 0 && val3 != 0 && val4 == 0)
ans = val3;
else if(val1 == 0 && val2 == 0 && val3 == 0 && val4 != 0)
ans = val4;
else if(val1 == 0 && val2 != 0 && val3 == 0 && val4 != 0)
{
ans = (val4*alpha) + ((1-alpha)*val2);
if(ans == double((floor(ans)+ceil(ans)))/2)
ans = floor(ans);

}
else if(val1 != 0 && val2 != 0 && val3 == 0 && val4 == 0)
{
ans = (val2*beta) + ((1-beta)*val1);
if(ans == double((floor(ans)+ceil(ans)))/2)
ans = floor(ans);
}
else if(val1 == 0 && val2 == 0 && val3 != 0 && val4 != 0)
{
ans = (val4*beta) + ((1-beta)*val3);
if(ans == double((floor(ans)+ceil(ans)))/2)
ans = floor(ans);
//cout << " " << ans << endl;
}
else if(val1 != 0 && val2 == 0 && val3 != 0 && val4 == 0)
{
ans = (val3*alpha) + ((1-alpha)*val1);
if(ans == double((floor(ans)+ceil(ans)))/2)
ans = floor(ans);

}
else 
{
ans = (val1*(1-alpha)*(1-beta)) + (val2*(1-alpha)*beta) + (val3*alpha*(1-beta)) + (val4*alpha*beta);
if(ans == double((floor(ans)+ceil(ans))/2))
ans = floor(ans);
//cout << ans <<endl;
}
value = round(ans);
return value;

}













 
