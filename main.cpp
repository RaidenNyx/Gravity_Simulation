#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <GLFW/glfw3.h>
#include <ostream>
#include <vector>

int screenHeight=600,screenWidth = 800;
float g_pixels = 9.81f * 0.1f;

struct Particle{
    float posX,posY;
    float velX,velY;
    float mass;
    float radius;
    std::vector<std::pair<float,float>> trail;
    int maxTrailLength = 100;    
};

GLFWwindow * StartGLFW(){
    if(!glfwInit()){
        std::cerr<<"failed to initialize glfw" << std::endl;
        return nullptr;
    }
    GLFWwindow * window = glfwCreateWindow(800, 600, "gravity_simulator", NULL,NULL);
    glfwMakeContextCurrent(window);
    return window;
}

void DrawCircle(float centerX,float centerY,float radius , int res){ 
    glColor3f(1.0f, 1.0f,1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, centerY);
    for(int i =0; i <= res;i++){
        float angle = 2.0f * M_PI * ( static_cast<float>(i)/res);
        float x = centerX + cos(angle) * radius;
        float y = centerY + sin(angle) *  radius;
        glVertex2f(x,y);
    }
    glEnd();
}

void DrawTrail(const Particle&particle){
    glColor3f(1.0f,1.0f,1.0f);
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i < particle.trail.size();i++){
        float minBrightness = 0.3f;
        float maxBrightness = 1.0f;
        float brightness = minBrightness + (maxBrightness - minBrightness) * (static_cast<float>(i) / (particle.trail.size() - 1));
        glColor3f(brightness, brightness, brightness);
        glVertex2f(particle.trail[i].first,particle.trail[i].second);    
    }
    glEnd();
}

float CalculateDistance(const Particle&p1,const Particle&p2){
    return std::sqrt((pow((p2.posX - p1.posX),2)) + (pow((p2.posY-p1.posY),2)));
}

int main(){
    GLFWwindow*window = StartGLFW();
    if(!window){
        std::cerr << "Failed to create GLFW window \n";
        glfwTerminate();
        return -1;
    }
    int width,height;

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*win,int width,int height){
    screenWidth = static_cast<int>(width);
    screenHeight = static_cast<int>(height);
    glViewport(0, 0, width, height); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    });

    using Clock = std::chrono::high_resolution_clock;
    auto last_time = Clock::now();
    

    std::vector<Particle> particles;
    particles.push_back(Particle{400.0f,300.0f,0.0f,0.0f,500000.0f,30.0f});
    particles.push_back(Particle{500.0f,300.0f,0.0f,-200.0f,1000.0f,10.0f});
    particles.push_back(Particle{200.0f,400.0f,0.0f,-90.0f,2000.0f,15.0f});
        
    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT);

        auto now = Clock::now();
        std::chrono::duration<double> frameDur = now - last_time;
        double dt = frameDur.count();
        last_time = now;
        const double MAX_DT = 0.05;
        if(dt > MAX_DT) dt = MAX_DT;

        float fdt = static_cast<float>(dt);
        // for Gravity simulation on earth i.e not in free space
        // const float pixelspm = 100.0f;
        // const float g = 9.81f;
        // const float g_pixels = g * pixelspm;
        // for(auto &particle : particles){
        //     float accelX = 0.0f;
        //     float accelY = g_pixels;
        // 
        //     particle.velX += accelX * fdt;
        //     particle.velY += accelY * fdt;
        //
        //     particle.posX += particle.velX * fdt;
        //     particle.posY += particle.velY * fdt;
        //
        //     if (particle.posX + particle.radius > screenWidth){
        //         particle.posX = screenWidth - particle.radius;
        //         particle.velX = -particle.velX * 0.9f;
        //     }
        //     if (particle.posY + particle.radius > screenHeight) {
        //         particle.posY = screenHeight - particle.radius;
        //         particle.velY = -particle.velY * 0.9f;
        //     }
        //     if (particle.posX - particle.radius < 0) {
        //         particle.posX = particle.radius; 
        //         particle.velX = -particle.velX * 0.9f;
        //     }
        //     if (particle.posY - particle.radius < 0) {
        //         particle.posY = particle.radius;
        //         particle.velY = -particle.velY * 0.9f;
        //     }
        // }
        float G = 6.0f;
        for (int i = 0; i < particles.size();i++){
            for (int j = i + 1; j < particles.size();j++){
                float dx = particles[j].posX - particles[i].posX;
                float dy = particles[j].posY - particles[i].posY;
                float distance = std::sqrt(dx*dx+dy*dy);
                float normalX = dx/distance;
                float normalY = dy/distance;
                if (distance == 0.0f) continue; 
                float F = G * particles[i].mass * particles[j].mass/(distance*distance + 1e-6f);

                float ax1 = F/particles[i].mass * normalX;
                float ay1 = F/particles[i].mass * normalY;
                float ax2 = -F/particles[j].mass * normalX;
                float ay2 = -F/particles[j].mass * normalY;

                particles[i].velX += ax1 * fdt;
                particles[i].velY += ay1 * fdt;
                particles[j].velX += ax2 * fdt;
                particles[j].velY += ay2 * fdt;
            }
        }
        for(auto &particle : particles){
            particle.trail.push_back({particle.posX,particle.posY});
            if(particle.trail.size() > particle.maxTrailLength){
                particle.trail.erase(particle.trail.begin());
            }
            DrawTrail(particle);
            particle.posX += particle.velX * fdt;
            particle.posY += particle.velY * fdt;
        }
        for (int i = 0; i < particles.size();i++){
            for (int j = i + 1; j < particles.size();j++){
                float dx = particles[j].posX - particles[i].posX;
                float dy = particles[j].posY - particles[i].posY;
                float distance = std::sqrt(dx*dx+dy*dy);
                float normalX = dx/distance;
                float normalY = dy/distance;

                if(distance ==0.0f) continue;
                float overlap = (particles[i].radius + particles[j].radius) - distance;

                if(overlap > 0){ 
                float seperateX = normalX * (overlap/2);
                float seperateY = normalY * (overlap/2);
                particles[i].posX -= seperateX;
                particles[i].posY -= seperateY;
                particles[j].posX += seperateX;
                particles[j].posY += seperateY;

                float vi_n = particles[i].velX * normalX + particles[i].velY * normalY;
                float vj_n = particles[j].velX * normalX + particles[j].velY * normalY;

                float mi = particles[i].mass;
                float mj = particles[j].mass;

                float vi_n_after_col = ((mi-mj)*vi_n + 2*mj*vj_n)/(mi+mj);
                float vj_n_after_col = ((mj-mi)*vj_n + 2*mi*vi_n)/(mi+mj);

                particles[i].velX += (vi_n_after_col - vi_n) * normalX;
                particles[i].velY += (vi_n_after_col - vi_n) * normalY;
                particles[j].velX += (vj_n_after_col - vj_n) * normalX;
                particles[j].velY += (vj_n_after_col - vj_n) * normalY;
                }
            }   
        }
        for(auto &particle : particles){
            DrawCircle(particle.posX,particle.posY,particle.radius,100);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();

    return 0;
}
