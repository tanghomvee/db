/*
Navicat MySQL Data Transfer

Source Server         : 172.26.20.99
Source Server Version : 50536
Source Host           : 172.26.20.99:3306
Source Database       : dbtool

Target Server Type    : MYSQL
Target Server Version : 50536
File Encoding         : 65001

Date: 2017-11-21 17:22:17
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `Course`
-- ----------------------------
DROP TABLE IF EXISTS `Course`;
CREATE TABLE `Course` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) DEFAULT NULL,
  `score` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of Course
-- ----------------------------
