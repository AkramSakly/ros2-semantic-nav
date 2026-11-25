from setuptools import setup

package_name = 'semantic_nav_commander'

setup(
    name=package_name,
    version='0.1.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Akram Sakly',
    maintainer_email='232819688+AkramSakly@users.noreply.github.com',
    description='Mission node that sends the robot to a named object.',
    license='MIT',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'go_to_object = semantic_nav_commander.go_to_object:main',
        ],
    },
)
