load ~/goby3-course/logs/usv/usv_29930516T214924.h5
close all;

% plot usv x/y
figure;
usv_nav = goby3_course__usv_nav_1.goby3_course_dccl_NavigationReport;
plot(usv_nav.x, usv_nav.y);
axis equal;
xlabel('X (m)')
ylabel('Y (m)')
title('USV position')

% plot auv depth
figure;
auv_nav = goby3_course__auv_nav_2.goby3_course_dccl_NavigationReport;
plot(auv_nav.z);
xlabel('message count')
ylabel('Z (m)')
title('AUV depth')

% plot ssp 
figure;
plot(auv_nav.soundspeed, auv_nav.z, 'o');
xlabel('sound speed (m/s)')
ylabel('Z (m)')
title('AUV sound speed profile')

