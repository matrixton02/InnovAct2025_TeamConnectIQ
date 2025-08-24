
       
        const observerOptions = {
            threshold: 0.3,
            rootMargin: '0px 0px -50px 0px'
        };

        const observer = new IntersectionObserver((entries) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    entry.target.classList.add('visible');
                    
                    const sectionIndex = parseInt(entry.target.id.split('-')[1]);
                    updateNavDots(sectionIndex);
                    updateBackground(sectionIndex);
                }
            });
        }, observerOptions);

       
        document.querySelectorAll('.section').forEach(section => {
            observer.observe(section);
        });

        
        function scrollToSection(index) {
            document.getElementById(`section-${index}`).scrollIntoView({
                behavior: 'smooth'
            });
        }

        function updateNavDots(activeIndex) {
            document.querySelectorAll('.nav-dot').forEach((dot, index) => {
                dot.classList.toggle('active', index === activeIndex);
            });
        }

        function updateBackground(sectionIndex) {
            const body = document.body;
            
            // Remove all background classes
            body.classList.remove('bg-hero', 'bg-problem');
            
            // Add appropriate background class
            if (sectionIndex === 0) {
                body.classList.add('bg-hero');
            } else if (sectionIndex === 1) {
                body.classList.add('bg-problem');
            }
        }

 
        function typeWriter(element, text, speed = 50) {
            let i = 0;
            element.innerHTML = '';
            
            function type() {
                if (i < text.length) {
                    element.innerHTML += text.charAt(i);
                    i++;
                    setTimeout(type, speed);
                }
            }
            type();
        }

      
        document.addEventListener('DOMContentLoaded', () => {
       
            document.querySelectorAll('.feature-card').forEach(card => {
                card.addEventListener('mouseenter', () => {
                    card.style.transform = 'translateY(-10px) scale(1.02)';
                });
                
                card.addEventListener('mouseleave', () => {
                    card.style.transform = 'translateY(0) scale(1)';
                });
            });

            document.querySelectorAll('a[href^="#"]').forEach(anchor => {
                anchor.addEventListener('click', function (e) {
                    e.preventDefault();
                    document.querySelector(this.getAttribute('href')).scrollIntoView({
                        behavior: 'smooth'
                    });
                });
            });

        });

        setInterval(() => {
            const statusElements = document.querySelectorAll('.demo-terminal p');
            if (statusElements.length > 0) {
                const randomIndex = Math.floor(Math.random() * statusElements.length);
                const element = statusElements[randomIndex];
                element.style.color = '#00ff41';
                setTimeout(() => {
                    element.style.color = '#00ff41';
                }, 500);
            }
        }, 3000);