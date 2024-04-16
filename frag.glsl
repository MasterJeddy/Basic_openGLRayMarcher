#version 330 core
out vec4 FragColor;

uniform vec2 iResolution;
uniform vec3 camPos;
uniform vec3 redBallPos;
uniform vec3 greenBallPos;
uniform float blendingDistance;



const int MAX_STEPS = 255;
const float PERCISION = 0.001;
const float MAX_DISTANCE = 100;

struct surface {
    float distance;
    vec3 color;
};

float sdSphere( vec3 p, float s )
{
    return length(p)-s;
}


float sdPlane( vec3 p, vec3 n, float h )
{
    // n must be normalized
    return dot(p,n) + h;
}

vec2 smin( float a, float b, float k )
{
    float h = max( k-abs(a-b), 0.0 )/k;
    float m = h*h*0.5;
    float s = m*k*(1.0/2.0);
    return (a<b) ? vec2(a-s,m) : vec2(b-s,1.0-m);
}
surface distanceEstimarSecne(vec3 point){


    float sphere1 = sdSphere(point-greenBallPos,1.0);
    float sphere2 = sdSphere(point-redBallPos,1.0);

    float plane = sdPlane(point,normalize(vec3(0,1,0)),1);

    float outDistance = min(smin(sphere1,sphere2,blendingDistance).x,plane);
    vec3 outColor = mix(vec3(0,1.0,0.0),vec3(1.0,0,0),smin(sphere1,sphere2,blendingDistance).y);
    if (outDistance == plane){
        outColor = vec3(0.6);
    }

    return surface(outDistance,outColor);
}

vec3 surfaceNormal(vec3 point){
    float epsilon = 0.0005;
    return normalize(vec3(
                         distanceEstimarSecne(point+vec3(epsilon,0,0)).distance-distanceEstimarSecne(point-vec3(epsilon,0,0)).distance,
                         distanceEstimarSecne(point+vec3(0,epsilon,0)).distance-distanceEstimarSecne(point-vec3(0,epsilon,0)).distance,
                         distanceEstimarSecne(point+vec3(0,0,epsilon)).distance-distanceEstimarSecne(point-vec3(0,0,epsilon)).distance));
}

surface rayMarch(vec3 from,vec3 dir){

    float totalDistance = 0.0;
    int step =0;
    surface closetSurface;

    for (step=0;step<MAX_STEPS;step++){
        vec3 point = from+dir*totalDistance;
        closetSurface = distanceEstimarSecne(point);
        totalDistance += closetSurface.distance;
        if (closetSurface.distance < PERCISION) break;
        if (totalDistance>MAX_DISTANCE) break;
    }

    return surface(totalDistance,closetSurface.color);
}


void main()
{
    vec2 uv = gl_FragCoord.xy/iResolution;
    uv -= 0.5;
    uv.x *= iResolution.x/iResolution.y;

    vec3 col = vec3(0.6);

    vec3 camDir = normalize(vec3(uv,-1));

    surface surfaceFound = rayMarch(camPos,camDir);



    if (surfaceFound.distance < MAX_DISTANCE){
        vec3 point = camPos +camDir*surfaceFound.distance;
        vec3 normal = surfaceNormal(point);
        vec3 lightPos = vec3(2,2,4);
        vec3 lighDir = normalize(lightPos-point);

        float diffuse = clamp(dot(normal,lighDir),0.0,1.0);

        col = surfaceFound.color*diffuse;
    }


    FragColor = vec4(col, 1.0f);
}
