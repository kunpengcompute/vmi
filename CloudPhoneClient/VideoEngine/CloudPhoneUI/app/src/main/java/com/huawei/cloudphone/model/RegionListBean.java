/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.model;

import java.util.List;

/**
 * 创建设备时区域信息
 *
 * @author zhh
 * @since 2019-09-24
 */
public class RegionListBean extends Response {
    private List<RegionBean> info;

    public List<RegionBean> getInfo() {
        return info;
    }

    public void setInfo(List<RegionBean> info) {
        this.info = info;
    }

    @Override
    public String toString() {
        return "RegionListBean{" +
                "info=" + info +
                '}';
    }

    /**
     * 区域实体类
     */
    public static class RegionBean {
        private List<ClustersBean> clusters;
        private String name;

        public List<ClustersBean> getClusters() {
            return clusters;
        }

        public void setClusters(List<ClustersBean> clusters) {
            this.clusters = clusters;
        }

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }

        @Override
        public String toString() {
            return "RegionBean{" +
                    "clusters=" + clusters +
                    ", name='" + name + '\'' +
                    '}';
        }

        /**
         * 内部类
         */
        public static class ClustersBean {
            private int totalNum;
            private String alias;
            private int maxPodNum;
            private int id;
            private int usedNum;

            public int getTotalNum() {
                return totalNum;
            }

            public void setTotalNum(int totalNum) {
                this.totalNum = totalNum;
            }

            public String getAlias() {
                return alias;
            }

            public void setAlias(String alias) {
                this.alias = alias;
            }

            public int getMaxPodNum() {
                return maxPodNum;
            }

            public void setMaxPodNum(int maxPodNum) {
                this.maxPodNum = maxPodNum;
            }

            public int getId() {
                return id;
            }

            public void setId(int id) {
                this.id = id;
            }

            public int getUsedNum() {
                return usedNum;
            }

            public void setUsedNum(int usedNum) {
                this.usedNum = usedNum;
            }

            @Override
            public String toString() {
                return "ClustersBean{" +
                        "totalNum=" + totalNum +
                        ", alias='" + alias + '\'' +
                        ", maxPodNum=" + maxPodNum +
                        ", id=" + id +
                        ", usedNum=" + usedNum +
                        '}';
            }
        }
    }
}
